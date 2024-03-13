#include "JsonParameters.h"

#define _EXPORTING
#include "JsonRPCClient.h"

JsonRPCClient::JsonRPCClient(Transport::TransportType transport, const string& serverAddr) {
	m_client = make_unique<RPCClient>(transport, serverAddr);
}

#ifdef WIN32
DECLSPEC 
#endif
uint64_t CreateRpcClient(Transport::TransportType transport, const char* serverAddrP) {
	return (uint64_t)new JsonRPCClient(transport, serverAddrP);
}

#ifdef WIN32
DECLSPEC 
#endif
AsyncID  RpcCallAsync(uint64_t clientId, const char* jsonCallP, char* jsonCallResultP, size_t jsonCallResultLen, AsyncJsonReplyProcedure replyProcP) {
	auto client = (JsonRPCClient*)clientId;
	return client->RpcCallAsync(jsonCallP, jsonCallResultP, jsonCallResultLen, replyProcP);
}

#ifdef WIN32
DECLSPEC 
#endif
uint64_t RpcCall(uint64_t clientId, const char* jsonCallP, char* jsonCallResultP, size_t jsonCallResultLen) {
	auto client = (JsonRPCClient*)clientId;
	return client->RpcCall(jsonCallP, jsonCallResultP, jsonCallResultLen);
}

#ifdef WIN32
DECLSPEC 
#endif
void  DestroyRpcClient(uint64_t clientId) {
	delete (JsonRPCClient*)clientId;
}

mutex JsonRPCClient::m_asyncParamsMutex;
unordered_map<AsyncID, shared_ptr<JsonRPCClient::AsyncJsonCallParams>> JsonRPCClient::m_asyncParams;

void JsonRPCClient::AsyncRpcReplyProc(AsyncID asyncId, unsigned long result) {
	// make sure the asyncId was returned and the parameters block built BEFORE
	// the RpcAsync reply proc was called....
	unordered_map<AsyncID, shared_ptr<JsonRPCClient::AsyncJsonCallParams>>::iterator i;
	for (;;) {
		{
			unique_lock<mutex> lock(m_asyncParamsMutex);
			i = m_asyncParams.find(asyncId);
			if (i != m_asyncParams.end())
				break;
			std::this_thread::sleep_for(5ms);
		}
	};

	// build the result json.
	string jsonResult;
	if (JsonParameters::BuildJsonFromResultParameters(i->second->m_function, i->second->m_params, jsonResult)) {
		// copy the result json into the result buffer if it is long enough, else, return an error.
		if (i->second->m_jsonCallResultLen > jsonResult.size()) {
			memcpy(i->second->m_jsonCallResultP, jsonResult.c_str(), jsonResult.length() + 1);
		}
	}

#ifdef JSONRPCCLIENT_TRACES
	LogVText(JSONRPCCLIENT_MODULE, 0, true, "AsyncRpcReplyProc got JSON result : %s", jsonResult.c_str());
#endif

	// we now can free the parameter copies
	JsonParameters::CleanupParameters(i->second->m_params);

	// invoke the callback, passing it the async id so the client catch find back the corresponding async call and the parameters/results
	(i->second->m_replyProcP)(asyncId, result);

	{
		// free the the async params we don't need anymore
		unique_lock<mutex> lock(m_asyncParamsMutex);
		m_asyncParams.erase(asyncId);
	}
}

AsyncID	JsonRPCClient::RpcCallAsync(const char* jsonCallP, char* jsonCallResultP, size_t jsonCallResultLen, AsyncJsonReplyProcedure replyProcP) {
#ifdef JSONRPCCLIENT_TRACES
	LogVText(JSONRPCCLIENT_MODULE, 0, true, "AsyncRpcCall will process JSON : %s", jsonCallP);
#endif

	// analyze the json call and build the parameters vector
	string function;
	shared_ptr<vector<RemoteProcedureCall::ParameterBase*>> params = make_shared<vector<RemoteProcedureCall::ParameterBase*>>();
	if (!JsonParameters::BuildParametersFromJson(jsonCallP, function, params))
		return INVALID_ASYNC_ID; 

	// invoke the rpc client.
	AsyncID asyncId;
	{
		unique_lock<mutex> lock(m_asyncParamsMutex);
		asyncId = m_client->RpcCallAsync(JsonRPCClient::AsyncRpcReplyProc, function, params.get());
		m_asyncParams[asyncId] = make_shared<AsyncJsonCallParams>(function, jsonCallResultP, jsonCallResultLen, replyProcP, params);
	}

	return asyncId;
}

unsigned long JsonRPCClient::RpcCall(const char* jsonCallP, char* jsonCallResultP, size_t jsonCallResultLen) {
	unsigned long result = -1;
	if (jsonCallResultLen) 
		*jsonCallResultP = '\0';

#ifdef JSONRPCCLIENT_TRACES
	LogVText(JSONRPCCLIENT_MODULE, 0, true, "RpcCall will process JSON : %s", jsonCallP);
#endif

	// analyze the json call and build the parameters vector
	string function;
	shared_ptr<vector<RemoteProcedureCall::ParameterBase*>> params = make_shared<vector<RemoteProcedureCall::ParameterBase*>>();
	if (!JsonParameters::BuildParametersFromJson(jsonCallP, function, params))
		return -1; // #### TODO : find a proper way to return an error.

	// invoke the rpc client.
	ReturnValue<unsigned long, CommunicationErrors> r;
	if ((r = m_client->RpcCall(function, params.get())).IsError())
		return -1; // #### TODO : find a proper way to return an error.

	result = (unsigned long)r;

	// build the result json.
	string jsonResult;
	if (JsonParameters::BuildJsonFromResultParameters(function, params, jsonResult)) {
		// copy the result json into the result buffer if it is long enough, else, return an error.
		if (jsonCallResultLen > jsonResult.size()) {
			memcpy(jsonCallResultP, jsonResult.c_str(), jsonResult.length() + 1);
		}
	}

#ifdef JSONRPCCLIENT_TRACES
	LogVText(JSONRPCCLIENT_MODULE, 0, true, "RpcCall got JSON result : %s", jsonResult.c_str());
#endif

	// we now can free the parameter copies
	JsonParameters::CleanupParameters(params);

	return result;
}
