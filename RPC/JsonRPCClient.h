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
DECLSPEC uint64_t CreateRpcClient(Transport::TransportType transport, const char *serverAddrP);
DECLSPEC AsyncID  AsyncRpcCall(uint64_t client_id, const char* jsonCallP, char* jsonCallResultP, size_t jsonCallResultLen, AsyncJsonReplyProcedure* replyProcP);
DECLSPEC uint64_t RpcCall(uint64_t client_id, const char* jsonCallP, char* jsonCallResultP, size_t jsonCallResultLen);
DECLSPEC void	  DestroyRpcClient(uint64_t clientId);
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

	struct AsyncJsonCallParams {
		string													m_function;
		char*													m_jsonCallResultP;
		size_t													m_jsonCallResultLen;
		AsyncJsonReplyProcedure*								m_replyProcP;
		shared_ptr<vector<RemoteProcedureCall::ParameterBase*>> m_params;

		AsyncJsonCallParams(string& function, char* jsonCallResultP, size_t jsonCallResultLen, AsyncJsonReplyProcedure* replyProcP, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>> params) {
			m_function = function;
			m_jsonCallResultP = jsonCallResultP;
			m_jsonCallResultLen = jsonCallResultLen;
			m_replyProcP = replyProcP;
			m_params = params;
		}
	};

	unique_ptr<RPCClient> m_client;
	static mutex												   m_asyncParamsMutex;
	static unordered_map<AsyncID, shared_ptr<AsyncJsonCallParams>> m_asyncParams;

	static void AsyncRpcReplyProc(AsyncID asyncId, unsigned long result);

	static bool BuildParametersFromJson(const char* json_callP, string& function, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>>& params);
	static bool BuildJsonFromParameters(string& function, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>>& params, string& json_result);

public:
	JsonRPCClient(Transport::TransportType transport, const string& serverAddr);

	AsyncID	 AsyncRpcCall(const char* jsonCallP, char* jsonCallResultP, size_t jsonCallResultLen, AsyncJsonReplyProcedure* replyProcP);
	unsigned long RpcCall(const char* jsonCallP, char* jsonCallResultP, size_t jsonCallResultLen);
};

#endif /* _JSON_RPC_H */
#pragma once
