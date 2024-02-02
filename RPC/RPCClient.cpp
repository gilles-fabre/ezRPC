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
	unsigned long result;

#ifdef RPCCLIENT_TRACES
	LogVText(RPCCLIENT_MODULE, 0, true, "RPCClient::RpcCall(%s)", func_name.c_str());
#endif

	if (!m_rpcP) {
		cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: RemoteProcedureCall object couldn't be created!" << endl;
		return 0;
	}

#ifdef RPCCLIENT_TRACES
	std::chrono::time_point<std::chrono::system_clock> start, end;
	start = std::chrono::system_clock::now();
#endif

	std::chrono::duration<double> elapsed_seconds = end - start;
	va_list vl;
	va_start(vl, func_name);
	result = m_rpcP->SerializeCall(func_name, vl);
	va_end(vl);

#ifdef RPCCLIENT_TRACES
	end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed = end - start;
	LogVText(RPCCLIENT_MODULE, 4, true, "RpcCall executed in %f second(s)", elapsed.count());
#endif

	return result;
}

/**
 * \fn void RPCClient::AsyncRpcCallHandler(void* paramP)
 * \brief This is the function called by the spawned thread to process the async rpn call.
 *
 * \param paramP contains a pointer to a newly allocated (per thread) AsyncReplyHandlerParameters.
 */
void RPCClient::AsyncRpcCallHandler(void* paramP) {
	AsyncReplyHandlerParameters* p = (AsyncReplyHandlerParameters*)paramP;
	if (!p)
		return;

#ifdef RPCCLIENT_TRACES
	LogVText(RPCCLIENT_MODULE, 0, true, "RPCClient::RpcCallAsync(%s)", p->m_function.c_str());
#endif

	if (!p->m_rpcP) {
		cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: RemoteProcedureCall object couldn't be created!" << endl;
		return;
	}

#ifdef RPCCLIENT_TRACES
	std::chrono::time_point<std::chrono::system_clock> start, end;
	start = std::chrono::system_clock::now();
#endif

	std::chrono::duration<double> elapsed_seconds = end - start;
	unsigned long result = p->m_rpcP->SerializeCall(p->m_function, p->m_args);
	va_end(p->m_args);

#ifdef RPCCLIENT_TRACES
	end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed = end - start;
	LogVText(RPCCLIENT_MODULE, 4, true, "RpcCallAsync executed in %f second(s)", elapsed.count());
#endif


	// call back async procedure result handler
	AsyncReplyProcedure* procedureP = p->m_procedureP;
	unsigned long				 asyncId = p->m_asyncId;

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
	ss << func_name << "-" << std::chrono::system_clock::now().time_since_epoch().count();
	unsigned long asyncId = (unsigned long)std::hash<std::string>{}(ss.str());
	
	va_list vl, vlc;
	va_start(vl, func_name);
	va_copy(vlc, vl);

	std::shared_ptr<Thread> thread = std::make_shared<Thread>(&RPCClient::AsyncRpcCallHandler);
	thread->Run(new RPCClient::AsyncReplyHandlerParameters(m_rpcP, procedureP, asyncId, thread, func_name, vlc));

	va_end(vl);

	return asyncId;
}
