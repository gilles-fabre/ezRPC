#include <iostream>
#include <cstdarg>

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

	va_list vl;
	va_start(vl, func_name);
	result = m_rpcP->SerializeCall(func_name, vl);
	va_end(vl);

	return result;
}
