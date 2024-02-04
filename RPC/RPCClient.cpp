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
 * \brief see RemoteProcedureCall::SerializedCall ...
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

	shared_ptr<vector<unsigned char>> serialized_call = make_shared<vector<unsigned char>>();
	shared_ptr<unsigned long> result = make_shared<unsigned long>();
	va_list vl;
	va_start(vl, func_name);
	m_rpcP->PrepareSerializeCall(*serialized_call.get(), result.get(), func_name, vl);
	va_end(vl);

	{
		unique_lock lock(m_mutex);
		m_rpcP->SendSerializedCall(*(serialized_call.get()));
	}

	#ifdef RPCCLIENT_TRACES
	end = chrono::system_clock::now();
	chrono::duration<double> elapsed = end - start;
	LogVText(RPCCLIENT_MODULE, 4, true, "RpcCall executed in %f second(s)", elapsed.count());
#endif

	return *result.get();
}

/**
 * \fn void RPCClient::AsyncRpcCallHandler(void* paramP)
 * \brief This is the function called by the spawned thread to process the async rpn call.
 *
 * \param paramP contains a pointer to a newly allocated (per thread) AsyncReplyHandlerParameters.
 */
void RPCClient::AsyncRpcCallHandler(void* paramP) {
	AsyncRpcParameters* p = (AsyncRpcParameters*)paramP;
	if (!p)
		return;

#ifdef RPCCLIENT_TRACES
	LogText(RPCCLIENT_MODULE, 0, true, "RPCClient::RpcCallAsync");
#endif

	if (!p->m_rpcP) {
		cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: RemoteProcedureCall object couldn't be created!" << endl;
		return;
	}

#ifdef RPCCLIENT_TRACES
	chrono::time_point<chrono::system_clock> start, end;
	start = chrono::system_clock::now();
#endif

	{
		unique_lock lock(*(p->m_mutexP));
		p->m_rpcP->SendSerializedCall(*(p->m_serialized_call.get()));
	}

#ifdef RPCCLIENT_TRACES
	end = chrono::system_clock::now();
	chrono::duration<double> elapsed = end - start;
	LogVText(RPCCLIENT_MODULE, 4, true, "RpcCallAsync executed in %f second(s)", elapsed.count());
#endif


	// call back async procedure result handler
	AsyncReplyProcedure* procedureP = p->m_procedureP;
	unsigned long		 asyncId = p->m_asyncId;
	unsigned long		 result = *p->m_result;
	delete p;

	(*procedureP)(asyncId, result);
}

/**
 * \fn unsigned long RPCClient::RpcCallAsync(string func_name, ...)
 * \brief see RemoteProcedureCall::SerializedCall ...
 * 
 * WARNING : ALL PASSED PARAMETERS MUST BE KEPT ALIVE/IN SCOPE UNTIL THE
 *           CALLBACK PROCEDURE IS CALLED!
 *
 * \param procedureP is the callback method called upon asynchronous call completion
 * \param func_name is the name of the Remote Procedure called
 * \param ... variadic, is a set of param types and paramters, ended by END_OF_CALL
 *
 * \return the asynchronous call identified, also passed to the callback procedure upon async rpc completion.
 */
unsigned long RPCClient::RpcCallAsync(AsyncReplyProcedure* procedureP, string func_name, ...) {
	// build the asyncId
	stringstream ss;
	ss << func_name << "-" << chrono::system_clock::now().time_since_epoch().count();
	unsigned long asyncId = (unsigned long)hash<string>{}(ss.str());
	
	shared_ptr<Thread> thread = make_shared<Thread>(&RPCClient::AsyncRpcCallHandler);

	shared_ptr<vector<unsigned char>> serialized_call = make_shared<vector<unsigned char>>();
	shared_ptr<unsigned long> result = make_shared<unsigned long>();
	va_list vl;
	va_start(vl, func_name);
	m_rpcP->PrepareSerializeCall(*(serialized_call.get()), result.get(), func_name, vl);
	va_end(vl);

	thread->Run(new RPCClient::AsyncRpcParameters(&m_mutex,
												  m_rpcP,
												  procedureP,
												  asyncId,
												  thread,
												  result,
												  serialized_call));

	return asyncId;
}
