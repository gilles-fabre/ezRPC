#define _EXPORTING
#include "JsonRPCClient.h"

#include "nlohmann/json.hpp"

using json = nlohmann::json;

JsonRPCClient::JsonRPCClient(Transport::TransportType transport, const string& serverAddr) {
	m_client = make_unique<RPCClient>(transport, serverAddr);
}

DECLSPEC uint64_t CreateRpcClient(Transport::TransportType transport, const char* serverAddrP) {
	return (uint64_t)new JsonRPCClient(transport, serverAddrP);
}

DECLSPEC AsyncID  AsyncRpcCall(uint64_t client_id, const char* json_call, char* json_call_result, size_t jsonCallResultLen, AsyncJsonReplyProcedure reply_proc) {
	auto client = (JsonRPCClient*)client_id;
	return client->AsyncRpcCall(json_call, json_call_result, jsonCallResultLen, reply_proc);
}

DECLSPEC uint64_t RpcCall(uint64_t client_id, const char* json_call, char* json_call_result, size_t jsonCallResultLen) {
	auto client = (JsonRPCClient*)client_id;
	return client->RpcCall(json_call, json_call_result, jsonCallResultLen);
}

DECLSPEC void  DestroyRpcClient(uint64_t client_id) {
	delete (JsonRPCClient*)client_id;
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
			Sleep(5);
		}
	};

	// build the result json.
	string json_result;
	BuildJsonFromParameters(i->second->m_function, i->second->m_params, json_result);

	// copy the result json into the result buffer if it is long enough, else, return an error.
	if (i->second->m_jsonCallResultLen > json_result.size()) {
		strncpy_s(i->second->m_jsonCallResultP, i->second->m_jsonCallResultLen, json_result.c_str(), json_result.size());
	}

	(*(i->second->m_replyProcP))(asyncId, i->second->m_jsonCallResultP);

	{
		unique_lock<mutex> lock(m_asyncParamsMutex);
		m_asyncParams.erase(asyncId);
	}
}

AsyncID	JsonRPCClient::AsyncRpcCall(const char* json_callP, char* jsonCallResultP, size_t jsonCallResultLen, AsyncJsonReplyProcedure* replyProcP) {
	// analyze the json call and build the parameters vector
	string function;
	shared_ptr<vector<RemoteProcedureCall::ParameterBase*>> params = make_shared<vector<RemoteProcedureCall::ParameterBase*>>();
	if (!BuildParametersFromJson(json_callP, function, params))
		return -1;

	// invoke the rpc client.
	AsyncID asyncId;
	{
		unique_lock<mutex> lock(m_asyncParamsMutex);
		asyncId = m_client->RpcCallAsync(JsonRPCClient::AsyncRpcReplyProc, function, params.get());
		m_asyncParams[asyncId] = make_shared<AsyncJsonCallParams>(function, jsonCallResultP, jsonCallResultLen, replyProcP, params);
	}

	return asyncId;
}

unsigned long JsonRPCClient::RpcCall(const char* json_call, char* json_call_result, size_t jsonCallResultLen) {
	unsigned long result = -1;

	// analyze the json call and build the parameters vector
	string function;
	shared_ptr<vector<RemoteProcedureCall::ParameterBase*>> params = make_shared<vector<RemoteProcedureCall::ParameterBase*>>();
	if (!BuildParametersFromJson(json_call, function, params))
		return -1;

	// invoke the rpc client.
	result = m_client->RpcCall(function, params.get());

	// build the result json.
	string json_result;
	BuildJsonFromParameters(function, params, json_result);

	// copy the result json into the result buffer if it is long enough, else, return an error.
	if (jsonCallResultLen > json_result.size()) {
		strncpy_s(json_call_result, jsonCallResultLen, json_result.c_str(), json_result.size());
	}

	return result;
}


bool JsonRPCClient::BuildParametersFromJson(const char* json_call, string& function, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>>& params) {

	json call = json::parse(json_call);
	if (call.is_null() || !call.contains("function"))
		return false;

	function = call["function"];
	if (function.empty())
		return false;

	json jparams = call["parameters"];
	if (!jparams.is_null() && jparams.is_array() && !jparams.empty()) {
		for (auto param : jparams) {
			switch (RemoteProcedureCall::GetParameterType(param["type"])) {
				case RemoteProcedureCall::STRING: {
					string value = param["value"];
					params->push_back(new RemoteProcedureCall::Parameter<string>(value));
					break;
				}
			
				case RemoteProcedureCall::STRING_REF: {
					string* valueP = new string(param["value"]);
					params->push_back(new RemoteProcedureCall::Parameter<RemoteProcedureCall::ParamType>(RemoteProcedureCall::ParamType::PTR));
					params->push_back(new RemoteProcedureCall::Parameter<string>(*valueP, (uint64_t)valueP));
					break;
				}

				case RemoteProcedureCall::BYTE: {
					uint8_t value = param["value"];
					params->push_back(new RemoteProcedureCall::Parameter<uint8_t>(value));
					break;
				}
			
				case RemoteProcedureCall::BYTE_REF: {
					uint8_t* valueP = new uint8_t(param["value"]);
					params->push_back(new RemoteProcedureCall::Parameter<RemoteProcedureCall::ParamType>(RemoteProcedureCall::ParamType::PTR));
					params->push_back(new RemoteProcedureCall::Parameter<uint8_t>(*valueP, (uint64_t)valueP));
					break;
				}

				case RemoteProcedureCall::CHAR: {
					char value = (uint8_t)param["value"];
					params->push_back(new RemoteProcedureCall::Parameter<char>(value));
					break;
				}

				case RemoteProcedureCall::CHAR_REF: {
					uint8_t* valueP = new uint8_t(param["value"]);
					params->push_back(new RemoteProcedureCall::Parameter<RemoteProcedureCall::ParamType>(RemoteProcedureCall::ParamType::PTR));
					params->push_back(new RemoteProcedureCall::Parameter<char>(*((char*)valueP), (uint64_t)valueP));
					break;
				}

				case RemoteProcedureCall::UINT16: {
					uint16_t value = param["value"];
					params->push_back(new RemoteProcedureCall::Parameter<uint16_t>(value));
					break;
				}
			
				case RemoteProcedureCall::UINT16_REF: {
					uint16_t* valueP = new uint16_t(param["value"]);
					params->push_back(new RemoteProcedureCall::Parameter<RemoteProcedureCall::ParamType>(RemoteProcedureCall::ParamType::PTR));
					params->push_back(new RemoteProcedureCall::Parameter<uint16_t>(*valueP, (uint64_t)valueP));
					break;
				}

				case RemoteProcedureCall::UINT32: {
					uint32_t value = param["value"];
					params->push_back(new RemoteProcedureCall::Parameter<uint32_t>(value));
					break;
				}

				case RemoteProcedureCall::UINT32_REF: {
					uint32_t* valueP = new uint32_t(param["value"]);
					params->push_back(new RemoteProcedureCall::Parameter<RemoteProcedureCall::ParamType>(RemoteProcedureCall::ParamType::PTR));
					params->push_back(new RemoteProcedureCall::Parameter<uint32_t>(*valueP, (uint64_t)valueP));
					break;
				}

				case RemoteProcedureCall::UINT64: {
					uint64_t value = param["value"];
					params->push_back(new RemoteProcedureCall::Parameter<uint64_t>(value));
					break;
				}

				case RemoteProcedureCall::UINT64_REF: {
					uint64_t* valueP = new uint64_t(param["value"]);
					params->push_back(new RemoteProcedureCall::Parameter<RemoteProcedureCall::ParamType>(RemoteProcedureCall::ParamType::PTR));
					params->push_back(new RemoteProcedureCall::Parameter<uint64_t>(*valueP, (uint64_t)valueP));
					break;
				}
			
				case RemoteProcedureCall::INT16: {
					int16_t value = param["value"];
					params->push_back(new RemoteProcedureCall::Parameter<int16_t>(value));
					break;
				}
			
				case RemoteProcedureCall::INT16_REF: {
					int16_t* valueP = new int16_t(param["value"]);
					params->push_back(new RemoteProcedureCall::Parameter<RemoteProcedureCall::ParamType>(RemoteProcedureCall::ParamType::PTR));
					params->push_back(new RemoteProcedureCall::Parameter<int16_t>(*valueP, (uint64_t)valueP));
					break;
				}

				case RemoteProcedureCall::INT32: {
					int32_t value = param["value"];
					params->push_back(new RemoteProcedureCall::Parameter<int32_t>(value));
					break;
				}
			
				case RemoteProcedureCall::INT32_REF: {
					int32_t* valueP = new int32_t(param["value"]);
					params->push_back(new RemoteProcedureCall::Parameter<RemoteProcedureCall::ParamType>(RemoteProcedureCall::ParamType::PTR));
					params->push_back(new RemoteProcedureCall::Parameter<int32_t>(*valueP, (uint64_t)valueP));
					break;
				}
			
				case RemoteProcedureCall::INT64: {
					int64_t value = param["value"];
					params->push_back(new RemoteProcedureCall::Parameter<int64_t>(value));
					break;
				}
			
				case RemoteProcedureCall::INT64_REF: {
					int64_t* valueP = new int64_t(param["value"]);
					params->push_back(new RemoteProcedureCall::Parameter<RemoteProcedureCall::ParamType>(RemoteProcedureCall::ParamType::PTR));
					params->push_back(new RemoteProcedureCall::Parameter<int64_t>(*valueP, (uint64_t)valueP));
					break;
				}

				case RemoteProcedureCall::DOUBLE: {
					double value = param["value"];
					params->push_back(new RemoteProcedureCall::Parameter<double>(value));
					break;
				}

				case RemoteProcedureCall::DOUBLE_REF: {
					double* valueP = new double(param["value"]);
					params->push_back(new RemoteProcedureCall::Parameter<RemoteProcedureCall::ParamType>(RemoteProcedureCall::ParamType::PTR));
					params->push_back(new RemoteProcedureCall::Parameter<double>(*valueP, (uint64_t)valueP));
					break;
				}
			}
		}
	}

	params->push_back(new RemoteProcedureCall::Parameter<RemoteProcedureCall::ParamType>(RemoteProcedureCall::ParamType::END_OF_CALL));

	return true;
}

bool JsonRPCClient::BuildJsonFromParameters(string& function, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>>& params, string& json_result) {
	// parse the parameters vector 'back' and build the result json
	json json_reply;
	json_reply["function"] = function;

	json parameters = json::array();

	if (params->size() > 1) { // we don't process the END_OF_CALL parameter
		int num_params = (int)params->size();
		for (int i = 0; i < num_params; i++) {
			RemoteProcedureCall::ParameterBase* paramP = params->at(i);
			json param;
			if (paramP->IsValidPointer()) {
				switch (paramP->GetType()) {
				case RemoteProcedureCall::ParamType::STRING:
					param["type"] = "STRING_REF";
					param["value"] = ParameterSafeCast(string, paramP)->GetReference(true);
					delete ((string*)paramP->GetCallerPointer());
					parameters += param;
					break;

				case RemoteProcedureCall::ParamType::BYTE:
					param["type"] = "BYTE_REF";
					param["value"] = ParameterSafeCast(uint8_t, paramP)->GetReference(true);
					delete ((uint8_t*)paramP->GetCallerPointer());
					parameters += param;
					break;

				case RemoteProcedureCall::ParamType::CHAR:
					param["type"] = "CHAR_REF";
					param["value"] = ParameterSafeCast(uint8_t, paramP)->GetReference(true);
					delete ((uint8_t*)paramP->GetCallerPointer());
					parameters += param;
					break;

				case RemoteProcedureCall::ParamType::UINT16:
					param["type"] = "UINT16_REF";
					param["value"] = ParameterSafeCast(uint16_t, paramP)->GetReference(true);
					delete ((uint16_t*)paramP->GetCallerPointer());
					parameters += param;
					break;

				case RemoteProcedureCall::ParamType::INT16:
					param["type"] = "INT16_REF";
					param["value"] = ParameterSafeCast(int16_t, paramP)->GetReference(true);
					delete ((int16_t*)paramP->GetCallerPointer());
					parameters += param;
					break;

				case RemoteProcedureCall::ParamType::UINT32:
					param["type"] = "UINT32_REF";
					param["value"] = ParameterSafeCast(uint32_t, paramP)->GetReference(true);
					delete ((uint32_t*)paramP->GetCallerPointer());
					parameters += param;
					break;

				case RemoteProcedureCall::ParamType::INT32:
					param["type"] = "INT32_REF";
					param["value"] = ParameterSafeCast(int32_t, paramP)->GetReference(true);
					delete ((int32_t*)paramP->GetCallerPointer());
					parameters += param;
					break;

				case RemoteProcedureCall::ParamType::UINT64:
					param["type"] = "UINT64_REF";
					param["value"] = ParameterSafeCast(uint64_t, paramP)->GetReference(true);
					delete ((uint64_t*)paramP->GetCallerPointer());
					parameters += param;
					break;

				case RemoteProcedureCall::ParamType::INT64:
					param["type"] = "INT64_REF";
					param["value"] = ParameterSafeCast(int64_t, paramP)->GetReference(true);
					delete ((int64_t*)paramP->GetCallerPointer());
					parameters += param;
					break;

				case RemoteProcedureCall::ParamType::DOUBLE:
					param["type"] = "DOUBLE_REF";
					param["value"] = ParameterSafeCast(double, paramP)->GetReference(true);
					delete ((double*)paramP->GetCallerPointer());
					parameters += param;
					break;
				}
			}
		}
	}

	json_reply["parameters"] = parameters;

	json_result = json_reply.dump();

	return true;
}

