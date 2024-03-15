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
 * \fn RpcReturnValue RPCClient::RpcCall(string function, ...)
 * \brief see RemoteProcedureCall::SerializedCall and RemoteProcedureCall::SendSerializedCall...
 * 
 * \param function is the name of the Remote Procedure called
 * \param ... variadic, is a set of param types and paramters, ended by END_OF_CALL
 * 
 * \return ReturnValue the Remote Procedure result if no error was encountered.
 */
RpcReturnValue RPCClient::RpcCall(string function, ...) {
	RpcReturnValue r;

#ifdef RPCCLIENT_TRACES
	LogVText(RPCCLIENT_MODULE, 0, true, "RPCClient::RpcCall(%s)", function.c_str());
#endif

	if (!m_rpcP) {
		cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: RemoteProcedureCall object couldn't be created!" << endl;
		r = RpcReturnValue { RemoteProcedureErrors::ErrorCode::CommunicationDropped };
		return r;
	}

#ifdef RPCCLIENT_TRACES
	chrono::time_point<chrono::system_clock> start, end;
	start = chrono::system_clock::now();
#endif

	vector<unsigned char>	serializedCall;
	uint64_t				result;
	va_list vl;
	va_start(vl, function);
	m_rpcP->PrepareSerializeCall(0, function, serializedCall, &result, vl);
	va_end(vl);

	ReturnValue<bool, CommunicationErrors> rv;
	if ((rv = m_rpcP->SendSerializedCall(0, serializedCall)).IsError()) {
		r = RpcReturnValue{(RemoteProcedureErrors::ErrorCode)rv};
		return r;
	}


	#ifdef RPCCLIENT_TRACES
	end = chrono::system_clock::now();
	chrono::duration<double> elapsed = end - start;
	LogVText(RPCCLIENT_MODULE, 4, true, "RpcCall executed in %f second(s)", elapsed.count());
#endif

	r = result;
	return r;
}

RpcReturnValue RPCClient::RpcCall(string function, vector<RemoteProcedureCall::ParameterBase*>* paramsP) {
	RpcReturnValue r;

#ifdef RPCCLIENT_TRACES
	LogVText(RPCCLIENT_MODULE, 0, true, "RPCClient::RpcCall(%s) - vector based", function.c_str());
#endif

	if (!m_rpcP) {
		cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: RemoteProcedureCall object couldn't be created!" << endl;

		r = RpcReturnValue{ RemoteProcedureErrors::ErrorCode::CommunicationDropped };
		return r;
	}

#ifdef RPCCLIENT_TRACES
	chrono::time_point<chrono::system_clock> start, end;
	start = chrono::system_clock::now();
#endif

	vector<unsigned char>	serializedCall;
	uint64_t				result;
	m_rpcP->PrepareSerializeCall(0, function, serializedCall, &result, paramsP);

	ReturnValue<bool, CommunicationErrors> rv;
	if ((rv = m_rpcP->SendSerializedCall(0, serializedCall)).IsError()) {
		r = RpcReturnValue{RemoteProcedureErrors::ErrorCode(rv)};
		return r;
	}

#ifdef RPCCLIENT_TRACES
	end = chrono::system_clock::now();
	chrono::duration<double> elapsed = end - start;
	LogVText(RPCCLIENT_MODULE, 4, true, "RpcCall executed in %f second(s)", elapsed.count());
#endif

	r = result;
	return r;
}

/**
 * \fn RpcReturnValue RPCClient::RpcCallAsync(string function, ...)
 * \brief see RemoteProcedureCall::SerializedCall and RemoteProcedureCall::SendSerializedCall...
 * 
 * WARNING : ALL PASSED PARAMETERS MUST BE KEPT ALIVE/IN SCOPE UNTIL THE
 *           CALLBACK PROCEDURE IS CALLED!
 *
 * \param procedureP is the callback method called upon asynchronous call completion
 * \param function is the name of the Remote Procedure called
 * \param ... variadic, is a set of param types and paramters, ended by END_OF_CALL
 *
 * \return (AsyncID)ReturnValue a valid (non 0) AsyncID, 0 else (and the
 *					error code is set).
 */
ReturnValue<AsyncID, CommunicationErrors> RPCClient::RpcCallAsync(AsyncReplyProcedure* procedureP, string function, ...) {
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

		r = { CommunicationErrors::ErrorCode::CommunicationDropped };
		return r;
	}

#ifdef RPCCLIENT_TRACES
	chrono::time_point<chrono::system_clock> start, end;
	start = chrono::system_clock::now();
#endif

	shared_ptr<vector<unsigned char>>	serializedCall = make_shared<vector<unsigned char>>();
	shared_ptr<uint64_t>				result = make_shared<uint64_t>();
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
	shared_ptr<thread> aThread = make_shared<thread>([&](AsyncID _asyncId, shared_ptr<vector<unsigned char>> _serializedCall, shared_ptr<uint64_t> _result) {
		aThread->detach();
		detachedSem.R();
		ReturnValue<bool, CommunicationErrors>	  rv;
		ReturnValue<AsyncID, CommunicationErrors> rt;

		if ((rv = m_rpcP->SendSerializedCall(_asyncId, *_serializedCall)).IsError()) 
			rt = {(CommunicationErrors::ErrorCode)rv};
		else {
			// this is the block call, the server has processed the service when we're unblocked
			unique_lock<mutex> lock(m_asyncProcsMutex);
			auto i = m_async_procs.find(_asyncId);
			if (i != m_async_procs.end()) {
				(*(i->second))(_asyncId, *_result);
				m_async_procs.erase(_asyncId);
			}

			rt = { asyncId, CommunicationErrors::ErrorCode::None };
		}

		return rt;
	}, asyncId, serializedCall, result);

	detachedSem.A();

#ifdef RPCCLIENT_TRACES
	end = chrono::system_clock::now();
	chrono::duration<double> elapsed = end - start;
	LogVText(RPCCLIENT_MODULE, 4, true, "RpcCallAsync %lu executed in %f second(s)", asyncId, elapsed.count());
#endif

	r = { asyncId, CommunicationErrors::ErrorCode::None };
	return r;
}

ReturnValue<AsyncID, CommunicationErrors> RPCClient::RpcCallAsync(AsyncReplyProcedure* procedureP, string function, vector<RemoteProcedureCall::ParameterBase*>* paramsP) {
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

		r = { CommunicationErrors::ErrorCode::CommunicationDropped };
		return r;
	}

#ifdef RPCCLIENT_TRACES
	chrono::time_point<chrono::system_clock> start, end;
	start = chrono::system_clock::now();
#endif

	shared_ptr<vector<unsigned char>>	serializedCall = make_shared<vector<unsigned char>>();
	shared_ptr<uint64_t>				result = make_shared<uint64_t>();
	m_rpcP->PrepareSerializeCall(asyncId, function, *serializedCall, result.get(), paramsP);

	Semaphore detachedSem(0);

	{
		unique_lock<mutex> lock(m_asyncProcsMutex);
		m_async_procs.insert(pair<AsyncID, AsyncReplyProcedure*>(asyncId, procedureP));
	}

	// create a thread which will wait for *a* deserialized reply
	// this reply can be associated with any previous async call,
	// hence we need to redirect it to the proper async reply procedure
	shared_ptr<thread> aThread = make_shared<thread>([&](AsyncID _asyncId, shared_ptr<vector<unsigned char>> _serializedCall, shared_ptr<uint64_t> _result) {
		aThread->detach();
		detachedSem.R();
		ReturnValue<bool, CommunicationErrors>	  rv;
		ReturnValue<AsyncID, CommunicationErrors> rt;

		if ((rv = m_rpcP->SendSerializedCall(_asyncId, *_serializedCall)).IsError())
			rt = { (CommunicationErrors::ErrorCode)rv };
		else {
			// this is the block call, the server has processed the service when we're unblocked
			{
				unique_lock<mutex> lock(m_asyncProcsMutex);
				auto i = m_async_procs.find(_asyncId);
				if (i != m_async_procs.end()) {
					(*(i->second))(_asyncId, *_result);
					m_async_procs.erase(_asyncId);
				}
			}
			rt = { asyncId, CommunicationErrors::ErrorCode::None };
		}

		return rt;
	}, asyncId, serializedCall, result);

	detachedSem.A();

#ifdef RPCCLIENT_TRACES
	end = chrono::system_clock::now();
	chrono::duration<double> elapsed = end - start;
	LogVText(RPCCLIENT_MODULE, 4, true, "RpcCallAsync %lu executed in %f second(s)", asyncId, elapsed.count());
#endif


	r = { asyncId, CommunicationErrors::ErrorCode::None };
	return r;
}
