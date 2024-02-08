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
 * \fn unsigned long RPCClient::RpcCall(string func_name, ...)
 * \brief see RemoteProcedureCall::SerializedCall and RemoteProcedureCall::SendSerializedCall...
 * 
 * \param func_name is the name of the Remote Procedure called
 * \param ... variadic, is a set of param types and paramters, ended by END_OF_CALL
 * 
 * \return the Remote Procedure result.
 */
unsigned long RPCClient::RpcCall(string func_name, ...) {

#ifdef RPCCLIENT_TRACES
	LogVText(RPCCLIENT_MODULE, 0, true, "RPCClient::RpcCall(%s)", func_name.c_str());
#endif

	if (!m_rpcP) {
		cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: RemoteProcedureCall object couldn't be created!" << endl;
		return 0;
	}

#ifdef RPCCLIENT_TRACES
	chrono::time_point<chrono::system_clock> start, end;
	start = chrono::system_clock::now();
#endif

	vector<unsigned char>	serialized_call;
	unsigned long			result;
	va_list vl;
	va_start(vl, func_name);
	m_rpcP->PrepareSerializeCall(0, func_name, serialized_call, &result, vl);
	va_end(vl);

	m_rpcP->SendSerializedCall(0, serialized_call);

	#ifdef RPCCLIENT_TRACES
	end = chrono::system_clock::now();
	chrono::duration<double> elapsed = end - start;
	LogVText(RPCCLIENT_MODULE, 4, true, "RpcCall executed in %f second(s)", elapsed.count());
#endif

	return result;
}

/**
 * \fn unsigned long RPCClient::RpcCallAsync(string func_name, ...)
 * \brief see RemoteProcedureCall::SerializedCall and RemoteProcedureCall::SendSerializedCall...
 * 
 * WARNING : ALL PASSED PARAMETERS MUST BE KEPT ALIVE/IN SCOPE UNTIL THE
 *           CALLBACK PROCEDURE IS CALLED!
 *
 * \param procedureP is the callback method called upon asynchronous call completion
 * \param func_name is the name of the Remote Procedure called
 * \param ... variadic, is a set of param types and paramters, ended by END_OF_CALL
 *
 * \return the asynchronous call identifier, also passed to the callback procedure upon async rpc completion.
 */
AsyncID RPCClient::RpcCallAsync(AsyncReplyProcedure* procedureP, string func_name, ...) {
	// build the asyncId
	stringstream ss;
	ss << func_name << "-" << chrono::system_clock::now().time_since_epoch().count();
	AsyncID asyncId = (AsyncID)hash<string>{}(ss.str());
	
#ifdef RPCCLIENT_TRACES
	LogVText(RPCCLIENT_MODULE, 0, true, "RPCClient::RpcCallAsync(%s) : %lu", func_name.c_str(), asyncId);
#endif

	if (!m_rpcP) {
		cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: RemoteProcedureCall object couldn't be created!" << endl;
		return 0;
	}

#ifdef RPCCLIENT_TRACES
	chrono::time_point<chrono::system_clock> start, end;
	start = chrono::system_clock::now();
#endif

	shared_ptr<vector<unsigned char>>	serialized_call = make_shared<vector<unsigned char>>();
	shared_ptr<unsigned long>			result = make_shared<unsigned long>();
	va_list vl;
	va_start(vl, func_name);
	m_rpcP->PrepareSerializeCall(asyncId, func_name, *serialized_call, result.get(), vl);
	va_end(vl);

	Semaphore detachedSem(0);

	{
		unique_lock<mutex> lock(m_async_procs_mutex);
		m_async_procs.insert(pair<AsyncID, AsyncReplyProcedure*>(asyncId, procedureP));
	}

	// create a thread which will wait for *a* deserialized reply
	// this reply can be associated with any previous async call,
	// hence we need to redirect it to the proper async reply procedure
	shared_ptr<thread> aThread = make_shared<thread>([&](AsyncID _asyncId, shared_ptr<vector<unsigned char>> _serialized_call, shared_ptr<unsigned long> _result) {
		aThread->detach();
		detachedSem.R();

		m_rpcP->SendSerializedCall(_asyncId, *_serialized_call); // this is the block call, the server has processed the service when we're unblocked

		{
			unique_lock<mutex> lock(m_async_procs_mutex);
			auto i = m_async_procs.find(_asyncId);
			if (i != m_async_procs.end()) {
				(*(i->second))(_asyncId, *_result);
				m_async_procs.erase(_asyncId);
			}
		}
	}, asyncId, serialized_call, result);

	detachedSem.A();

#ifdef RPCCLIENT_TRACES
	end = chrono::system_clock::now();
	chrono::duration<double> elapsed = end - start;
	LogVText(RPCCLIENT_MODULE, 4, true, "RpcCallAsync %lu executed in %f second(s)", asyncId, elapsed.count());
#endif

	return asyncId;
}
