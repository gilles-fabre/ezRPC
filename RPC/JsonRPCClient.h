#ifndef _JSON_RPC_CLIENT_H
#define _JSON_RPC_CLIENT_H

#include <log2reporter.h>

#include <RPCClient.h>

using namespace std;

#ifdef _DEBUG
#define JSONRPCCLIENT_TRACES 1
#endif

#define JSONRPCCLIENT_MODULE 0x10

#ifdef WIN32
#pragma warning(disable:4251)
#ifdef _EXPORTING
#define DECLSPEC __declspec(dllexport)
#else
#define DECLSPEC __declspec(dllimport)
#endif
#endif

/**
 * \JsonRPCClient C API
 * \brief Provides C APIs on top of RPCClient services
 */

typedef void AsyncJsonReplyProcedure(AsyncID, unsigned long result);

extern "C" {
#ifdef WIN32
DECLSPEC 
#endif
uint64_t CreateRpcClient(Transport::TransportType transport, const char *serverAddrP);
#ifdef WIN32
DECLSPEC 
#endif
AsyncID  RpcCallAsync(uint64_t clientId, const char* jsonCallP, char* jsonCallResultP, size_t jsonCallResultLen, AsyncJsonReplyProcedure* replyProcP);
#ifdef WIN32
DECLSPEC 
#endif
uint64_t RpcCall(uint64_t clientId, const char* jsonCallP, char* jsonCallResultP, size_t jsonCallResultLen);
#ifdef WIN32
DECLSPEC 
#endif
void	  DestroyRpcClient(uint64_t clientId);
};

/**
 * \class JsonRPCClient
 * \brief Provides the user with a high abstraction level Remote Procedure Calls service.
 */
#ifdef WIN32
class	DECLSPEC JsonRPCClient {
#else
class	JsonRPCClient {
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

	static bool BuildParametersFromJson(const char* jsonCallP, string& function, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>>& params);
	static bool BuildJsonFromParameters(string& function, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>>& params, string& jsonResult);
	static void CleanupParameters(shared_ptr<vector<RemoteProcedureCall::ParameterBase*>>& params);

public:
	JsonRPCClient(Transport::TransportType transport, const string& serverAddr);

	AsyncID	 RpcCallAsync(const char* jsonCallP, char* jsonCallResultP, size_t jsonCallResultLen, AsyncJsonReplyProcedure* replyProcP);
	unsigned long RpcCall(const char* jsonCallP, char* jsonCallResultP, size_t jsonCallResultLen);
};

#endif /* _JSON_RPC_CLIENT_H */
#pragma once
