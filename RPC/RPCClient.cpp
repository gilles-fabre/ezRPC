#include <iostream>
#include <cstdarg>
#include <chrono>
#include <sstream>
#include <functional>

#ifndef WIN32
#include <unistd.h>
#endif

#include <Thread.h>

#define _EXPORTING
#include "RPCClient.h"

/**
 * \fn unsigned long RPCClient::RpcCall(string function, ...)
 * \brief see RemoteProcedureCall::SerializedCall and RemoteProcedureCall::SendSerializedCall...
 * 
 * \param function is the name of the Remote Procedure called
 * \param ... variadic, is a set of param types and paramters, ended by END_OF_CALL
 * 
 * \return the Remote Procedure result.
 */
ReturnValue<unsigned long, CommunicationErrors>&& RPCClient::RpcCall(string function, ...) {
	ReturnValue<unsigned long, CommunicationErrors> r;

#ifdef RPCCLIENT_TRACES
	LogVText(RPCCLIENT_MODULE, 0, true, "RPCClient::RpcCall(%s)", function.c_str());
#endif

	if (!m_rpcP) {
		cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: RemoteProcedureCall object couldn't be created!" << endl;
		r = ReturnValue<unsigned long, CommunicationErrors>{0, CommunicationErrors::ErrorCode::CommunicationDropped};
		return std::move(r);
	}

#ifdef RPCCLIENT_TRACES
	chrono::time_point<chrono::system_clock> start, end;
	start = chrono::system_clock::now();
#endif

	vector<unsigned char>	serializedCall;
	unsigned long			result;
	va_list vl;
	va_start(vl, function);
	m_rpcP->PrepareSerializeCall(0, function, serializedCall, &result, vl);
	va_end(vl);

	if ((r = m_rpcP->SendSerializedCall(0, serializedCall)).IsError())
		return std::move(r);

	#ifdef RPCCLIENT_TRACES
	end = chrono::system_clock::now();
	chrono::duration<double> elapsed = end - start;
	LogVText(RPCCLIENT_MODULE, 4, true, "RpcCall executed in %f second(s)", elapsed.count());
#endif

	r = ReturnValue<unsigned long, CommunicationErrors>{ result, CommunicationErrors::ErrorCode::None };
	return std::move(r);
}

ReturnValue<unsigned long, CommunicationErrors>&& RPCClient::RpcCall(string function, vector<RemoteProcedureCall::ParameterBase*>* paramsP) {
	ReturnValue<unsigned long, CommunicationErrors> r;

#ifdef RPCCLIENT_TRACES
	LogVText(RPCCLIENT_MODULE, 0, true, "RPCClient::RpcCall(%s) - vector based", function.c_str());
#endif

	if (!m_rpcP) {
		cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: RemoteProcedureCall object couldn't be created!" << endl;

		r = ReturnValue<unsigned long, CommunicationErrors>{ 0, CommunicationErrors::ErrorCode::CommunicationDropped };
		return std::move(r);
	}

#ifdef RPCCLIENT_TRACES
	chrono::time_point<chrono::system_clock> start, end;
	start = chrono::system_clock::now();
#endif

	vector<unsigned char>	serializedCall;
	unsigned long			result;
	m_rpcP->PrepareSerializeCall(0, function, serializedCall, &result, paramsP);

	if ((r = m_rpcP->SendSerializedCall(0, serializedCall)).IsError())
		return std::move(r);

#ifdef RPCCLIENT_TRACES
	end = chrono::system_clock::now();
	chrono::duration<double> elapsed = end - start;
	LogVText(RPCCLIENT_MODULE, 4, true, "RpcCall executed in %f second(s)", elapsed.count());
#endif

	r = ReturnValue<unsigned long, CommunicationErrors>{ result, CommunicationErrors::ErrorCode::None };
	return std::move(r);
}

/**
 * \fn unsigned long RPCClient::RpcCallAsync(string function, ...)
 * \brief see RemoteProcedureCall::SerializedCall and RemoteProcedureCall::SendSerializedCall...
 * 
 * WARNING : ALL PASSED PARAMETERS MUST BE KEPT ALIVE/IN SCOPE UNTIL THE
 *           CALLBACK PROCEDURE IS CALLED!
 *
 * \param procedureP is the callback method called upon asynchronous call completion
 * \param function is the name of the Remote Procedure called
 * \param ... variadic, is a set of param types and paramters, ended by END_OF_CALL
 *
 * \return the asynchronous call identifier, also passed to the callback procedure upon async rpc completion.
 */
ReturnValue<AsyncID, CommunicationErrors>&& RPCClient::RpcCallAsync(AsyncReplyProcedure* procedureP, string function, ...) {
	ReturnValue<AsyncID, CommunicationErrors> r;

	// build the asyncId
	stringstream ss;
	ss << function << "-" << chrono::system_clock::now().time_since_epoch().count();
	AsyncID asyncId = (AsyncID)hash<string>{}(ss.str());
	
#ifdef RPCCLIENT_TRACES
	LogVText(RPCCLIENT_MODULE, 0, true, "RPCClient::RpcCallAsync(%s) : %lu", function.c_str(), asyncId);
#endif

	if (!m_rpcP) {
		cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: RemoteProcedureCall object couldn't be created!" << endl;

		r = ReturnValue<AsyncID, CommunicationErrors>{ 0, CommunicationErrors::ErrorCode::CommunicationDropped };
		return std::move(r);
	}

#ifdef RPCCLIENT_TRACES
	chrono::time_point<chrono::system_clock> start, end;
	start = chrono::system_clock::now();
#endif

	shared_ptr<vector<unsigned char>>	serializedCall = make_shared<vector<unsigned char>>();
	shared_ptr<unsigned long>			result = make_shared<unsigned long>();
	va_list vl;
	va_start(vl, function);
	m_rpcP->PrepareSerializeCall(asyncId, function, *serializedCall, result.get(), vl);
	va_end(vl);

	Semaphore detachedSem(0);

	{
		unique_lock<mutex> lock(m_asyncProcsMutex);
		m_async_procs.insert(pair<AsyncID, AsyncReplyProcedure*>(asyncId, procedureP));
	}

	// create a thread which will wait for *a* deserialized reply
	// this reply can be associated with any previous async call,
	// hence we need to redirect it to the proper async reply procedure
	shared_ptr<thread> aThread = make_shared<thread>([&](AsyncID _asyncId, shared_ptr<vector<unsigned char>> _serializedCall, shared_ptr<unsigned long> _result) {
		aThread->detach();
		detachedSem.R();

		if ((r = m_rpcP->SendSerializedCall(_asyncId, *_serializedCall)).IsError())  
			return std::move(r);

		{
			// this is the block call, the server has processed the service when we're unblocked
			unique_lock<mutex> lock(m_asyncProcsMutex);
			auto i = m_async_procs.find(_asyncId);
			if (i != m_async_procs.end()) {
				(*(i->second))(_asyncId, *_result);
				m_async_procs.erase(_asyncId);
			}
		}

		ReturnValue<AsyncID, CommunicationErrors> rt = ReturnValue<AsyncID, CommunicationErrors>{asyncId, CommunicationErrors::ErrorCode::None};
		return std::move(rt);
	}, asyncId, serializedCall, result);

	detachedSem.A();

#ifdef RPCCLIENT_TRACES
	end = chrono::system_clock::now();
	chrono::duration<double> elapsed = end - start;
	LogVText(RPCCLIENT_MODULE, 4, true, "RpcCallAsync %lu executed in %f second(s)", asyncId, elapsed.count());
#endif

	r = ReturnValue<AsyncID, CommunicationErrors>{ asyncId, CommunicationErrors::ErrorCode::None };
	return std::move(r);
}

ReturnValue<AsyncID, CommunicationErrors>&& RPCClient::RpcCallAsync(AsyncReplyProcedure* procedureP, string function, vector<RemoteProcedureCall::ParameterBase*>* paramsP) {
	ReturnValue<AsyncID, CommunicationErrors> r;

	// build the asyncId
	stringstream ss;
	ss << function << "-" << chrono::system_clock::now().time_since_epoch().count();
	AsyncID asyncId = (AsyncID)hash<string>{}(ss.str());

#ifdef RPCCLIENT_TRACES
	LogVText(RPCCLIENT_MODULE, 0, true, "RPCClient::RpcCallAsync(%s) - vector based : %lu", function.c_str(), asyncId);
#endif

	if (!m_rpcP) {
		cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: RemoteProcedureCall object couldn't be created!" << endl;

		r = ReturnValue<AsyncID, CommunicationErrors>{ 0, CommunicationErrors::ErrorCode::CommunicationDropped };
		return std::move(r);
	}

#ifdef RPCCLIENT_TRACES
	chrono::time_point<chrono::system_clock> start, end;
	start = chrono::system_clock::now();
#endif

	shared_ptr<vector<unsigned char>>	serializedCall = make_shared<vector<unsigned char>>();
	shared_ptr<unsigned long>			result = make_shared<unsigned long>();
	m_rpcP->PrepareSerializeCall(asyncId, function, *serializedCall, result.get(), paramsP);

	Semaphore detachedSem(0);

	{
		unique_lock<mutex> lock(m_asyncProcsMutex);
		m_async_procs.insert(pair<AsyncID, AsyncReplyProcedure*>(asyncId, procedureP));
	}

	// create a thread which will wait for *a* deserialized reply
	// this reply can be associated with any previous async call,
	// hence we need to redirect it to the proper async reply procedure
	shared_ptr<thread> aThread = make_shared<thread>([&](AsyncID _asyncId, shared_ptr<vector<unsigned char>> _serializedCall, shared_ptr<unsigned long> _result) {
		aThread->detach();
		detachedSem.R();

		if ((r = m_rpcP->SendSerializedCall(_asyncId, *_serializedCall)).IsError())
			return std::move(r);

		// this is the block call, the server has processed the service when we're unblocked
		{
			unique_lock<mutex> lock(m_asyncProcsMutex);
			auto i = m_async_procs.find(_asyncId);
			if (i != m_async_procs.end()) {
				(*(i->second))(_asyncId, *_result);
				m_async_procs.erase(_asyncId);
			}
		}

		ReturnValue<AsyncID, CommunicationErrors> rt = ReturnValue<AsyncID, CommunicationErrors>{ asyncId, CommunicationErrors::ErrorCode::None };
		return std::move(rt);
	}, asyncId, serializedCall, result);

	detachedSem.A();

#ifdef RPCCLIENT_TRACES
	end = chrono::system_clock::now();
	chrono::duration<double> elapsed = end - start;
	LogVText(RPCCLIENT_MODULE, 4, true, "RpcCallAsync %lu executed in %f second(s)", asyncId, elapsed.count());
#endif


	r = ReturnValue<AsyncID, CommunicationErrors>{ asyncId, CommunicationErrors::ErrorCode::None };
	return std::move(r);
}
