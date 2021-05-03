#include <iostream>
#include <cstdarg>
#include <chrono>

#ifndef WIN32
#include <unistd.h>
#endif

#include "RPCClient.h"

/**
 * \fn unsigned long RPCClient::RpcCall(string &func_name, int num_args, ...)
 * \brief see RemoteProcedureCall::SerializedCall ...
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
