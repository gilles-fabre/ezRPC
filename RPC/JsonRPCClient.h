#ifndef _JSON_RPC_H
#define _JSON_RPC_H

#include <log2reporter.h>

#include <RPCClient.h>

using namespace std;

#define JSONRPC_TRACES 1
#define JSONRPC_MODULE 0x10

#ifdef WIN32
#pragma warning(disable:4251)
#ifdef _EXPORTING
#define DECLSPEC __declspec(dllexport)
#else
#define DECLSPEC __declspec(dllimport)
#endif
#endif

/**
 * \RPCJson C API
 * \brief Provides C APIs on top of RPCClient services
 */

typedef void AsyncJsonReplyProcedure(AsyncID, char*);

extern "C" {
DECLSPEC uint64_t CreateRpcClient(Transport::TransportType transport, const char *server_addr);
DECLSPEC AsyncID  AsyncRpcCall(uint64_t client_id, const char* json_call, char* json_call_result, size_t json_call_result_len, AsyncJsonReplyProcedure reply_proc);
DECLSPEC uint64_t RpcCall(uint64_t client_id, const char* json_call, char* json_call_result, size_t json_call_result_len);
DECLSPEC void	  DestroyRpcClient(uint64_t client_id);
};

/**
 * \class RPCJsonClient
 * \brief Provides the user with a high abstraction level Remote Procedure Calls service.
 */
#ifdef WIN32
class	DECLSPEC JsonRPCClient {
#else
class	RPCJsonClient {
#endif
	unique_ptr<RPCClient> m_client;

	static void AsyncRpcReplyProc(AsyncID asyncId, unsigned long result);

	bool BuildParametersFromJson(const char* json_call, string& function, unique_ptr<vector<RemoteProcedureCall::ParameterBase*>>& params);
	bool BuildJsonFromParameters(string& function, unique_ptr<vector<RemoteProcedureCall::ParameterBase*>>& params, string& json_result);

public:
	JsonRPCClient(Transport::TransportType transport, const string& server_addr);

	AsyncID	 AsyncRpcCall(const char* json_call, char* json_call_result, size_t json_call_result_len, AsyncJsonReplyProcedure replyProc);
	unsigned long RpcCall(const char* json_call, char* json_call_result, size_t json_call_result_len);
};

#endif /* _JSON_RPC_H */
#pragma once
