#include "JsonParameters.h"

#define _EXPORTING
#include "JsonRPCServer.h"

JsonRPCServer::JsonRPCServer(Transport::TransportType transport, const string& serverAddr) {
	m_server = make_unique<RPCServer>(transport, serverAddr);
}

DECLSPEC void DestroyRpcServer(uint64_t serverId) {
	delete (JsonRPCServer*)serverId;
}

mutex JsonRPCServer::m_serverProcsMutex;
unordered_map<string, ServerProcedure*> JsonRPCServer::m_serverProcs;
unsigned long JsonRPCServer::JsonRPCServiceProc(string& name, vector<RemoteProcedureCall::ParameterBase*>* paramsP, void* user_dataP) {
	string			jsonCall;
	unsigned long	result = -1;
	
	// convert params to json
	shared_ptr<vector<RemoteProcedureCall::ParameterBase*>> params(paramsP);
	JsonParameters::BuildJsonFromParameters(name, params, jsonCall);

	// invoke origin procedure with json
	ServerProcedure* procP = nullptr;
	{
		unique_lock<mutex> lock(m_serverProcsMutex);
		auto p = m_serverProcs.find(name);
		if (p == m_serverProcs.end()) {
			procP = p->second;
			m_serverProcs.erase(name);
		}
	}
	if (procP) {
		char* jsonCallResult = new char[1024];
		result = (unsigned long)(*procP)(jsonCall.c_str(), jsonCallResult, sizeof(jsonCallResult));

		// converts result json to params.
		string jsonResult;
		JsonParameters::BuildParametersFromJson(jsonCallResult, name, params);
	}

	return result;
}

DECLSPEC void RegisterProcedure(uint64_t serverId, const char* name, ServerProcedure* procedureP) {
	auto server = (RPCServer*)serverId;

	server->RegisterProcedure(string(name), JsonRPCServer::JsonRPCServiceProc);
}
