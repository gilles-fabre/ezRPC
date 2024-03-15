#include "JsonParameters.h"

#define _EXPORTING
#include "JsonRPCServer.h"

JsonRPCServer::JsonRPCServer(Transport::TransportType transport, const string& serverAddr) {
	m_server = make_unique<RPCServer>(transport, serverAddr);
}

#ifdef WIN32
DECLSPEC 
#endif
uint64_t CreateRpcServer(Transport::TransportType transport, const char* serverAddrP) {
	return (uint64_t)new JsonRPCServer(transport, serverAddrP);
}

#ifdef WIN32
DECLSPEC 
#endif
void DestroyRpcServer(uint64_t serverId) {
	delete (JsonRPCServer*)serverId;
}

mutex JsonRPCServer::m_serverProcsMutex;
unordered_map<string, ServerProcedure*> JsonRPCServer::m_serverProcs;
RpcReturnValue JsonRPCServer::JsonRPCServiceProc(string& name, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>> params, void* user_dataP) {
	RpcReturnValue	r;
	string			jsonCall;
	uint64_t		result = -1;
	
	// convert params to json
	JsonParameters::BuildJsonFromCallParameters(name, params, jsonCall);

#ifdef JSONRPCSERVER_TRACES
	LogVText(JSONRPCSERVER_MODULE, 0, true, "JsonRPCServiceProc will process : %s", jsonCall.c_str());
#endif

	// invoke origin procedure with json
	ServerProcedure* procP = nullptr;
	{
		unique_lock<mutex> lock(JsonRPCServer::m_serverProcsMutex);
		auto p = JsonRPCServer::m_serverProcs.find(name);
		if (p != JsonRPCServer::m_serverProcs.end())
			procP = p->second;
	}

	if (procP) {
		unique_ptr<char[]> jsonCallResult = make_unique<char[]>(JSON_RESULT_MAX_SIZE);
		
		// invoke server's proc
		result = (uint64_t)(*procP)(jsonCall.c_str(), jsonCallResult.get(), JSON_RESULT_MAX_SIZE);

		// converts result json to params.
		params->clear();
		JsonParameters::BuildParametersFromJson(jsonCallResult.get(), name, params);
		
#ifdef JSONRPCSERVER_TRACES
		LogVText(JSONRPCSERVER_MODULE, 0, true, "received from JSON service procedure : %s", jsonCallResult.get());
#endif
	}

	r = result;
	return r;
}

#ifdef WIN32
DECLSPEC 
#endif
void RegisterProcedure(uint64_t serverId, const char* nameP, ServerProcedure* procedureP) {
	auto serverP = (JsonRPCServer*)serverId;
	string name(nameP);

	{
		unique_lock<mutex> lock(JsonRPCServer::m_serverProcsMutex);
		JsonRPCServer::m_serverProcs[name] = procedureP;
	}

	serverP->m_server->RegisterProcedure(name, JsonRPCServer::JsonRPCServiceProc);
}

#ifdef WIN32
DECLSPEC 
#endif
void UnregisterProcedure(uint64_t serverId, const char* nameP) {
	auto serverP = (JsonRPCServer*)serverId;
	string name(nameP);

	serverP->m_server->UnregisterProcedure(name);

	{
		unique_lock<mutex> lock(JsonRPCServer::m_serverProcsMutex);
		JsonRPCServer::m_serverProcs.erase(name);
	}
}

#ifdef WIN32
DECLSPEC 
#endif
void IterateAndWait(uint64_t serverId) {
	auto serverP = (JsonRPCServer*)serverId;
	serverP->m_server->IterateAndWait();
}

#ifdef WIN32
DECLSPEC 
#endif
void Stop(uint64_t serverId) {
	auto serverP = (JsonRPCServer*)serverId;
	serverP->m_server->Stop();
}
