#include "JsonParameters.h"

#define _EXPORTING
#include "JsonRPCServer.h"

JsonRPCServer::JsonRPCServer(Transport::TransportType transport, const string& serverAddr) {
	m_server = make_unique<RPCServer>(transport, serverAddr);
}

DECLSPEC uint64_t CreateRpcServer(Transport::TransportType transport, const char* serverAddrP) {
	return (uint64_t)new JsonRPCServer(transport, serverAddrP);
}

DECLSPEC void DestroyRpcServer(uint64_t serverId) {
	delete (JsonRPCServer*)serverId;
}

mutex JsonRPCServer::m_serverProcsMutex;
unordered_map<string, ServerProcedure*> JsonRPCServer::m_serverProcs;
unsigned long JsonRPCServer::JsonRPCServiceProc(string& name, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>> params, void* user_dataP) {
	string			jsonCall;
	unsigned long	result = -1;
	
	// convert params to json
	JsonParameters::BuildJsonFromCallParameters(name, params, jsonCall);

	// invoke origin procedure with json
	ServerProcedure* procP = nullptr;
	{
		unique_lock<mutex> lock(JsonRPCServer::m_serverProcsMutex);
		auto p = JsonRPCServer::m_serverProcs.find(name);
		if (p != JsonRPCServer::m_serverProcs.end())
			procP = p->second;
	}

	if (procP) {
		char* jsonCallResultP = new char[1024];
		
		// invoke server's proc
		result = (unsigned long)(*procP)(jsonCall.c_str(), jsonCallResultP, 1024);

		// converts result json to params.
		params->clear();
		JsonParameters::BuildParametersFromJson(jsonCallResultP, name, params);
		
		delete[] jsonCallResultP;
	}

	return result;
}

DECLSPEC void RegisterProcedure(uint64_t serverId, const char* nameP, ServerProcedure* procedureP) {
	auto serverP = (JsonRPCServer*)serverId;
	string name(nameP);

	{
		unique_lock<mutex> lock(JsonRPCServer::m_serverProcsMutex);
		JsonRPCServer::m_serverProcs[name] = procedureP;
	}

	serverP->m_server->RegisterProcedure(name, JsonRPCServer::JsonRPCServiceProc);
}

DECLSPEC void UnregisterProcedure(uint64_t serverId, const char* nameP) {
	auto serverP = (JsonRPCServer*)serverId;
	string name(nameP);

	serverP->m_server->UnregisterProcedure(name);

	{
		unique_lock<mutex> lock(JsonRPCServer::m_serverProcsMutex);
		JsonRPCServer::m_serverProcs.erase(name);
	}
}

DECLSPEC void IterateAndWait(uint64_t serverId) {
	auto serverP = (JsonRPCServer*)serverId;
	serverP->m_server->IterateAndWait();
}

DECLSPEC void Stop(uint64_t serverId) {
	auto serverP = (JsonRPCServer*)serverId;
	serverP->m_server->Stop();
}
