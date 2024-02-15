#define _EXPORTING
#include "JsonRPCClient.h"

#include "nlohmann/json.hpp"

using json = nlohmann::json;

JsonRPCClient::JsonRPCClient(Transport::TransportType transport, const string& server_addr) {
	m_client = make_unique<RPCClient>(transport, server_addr);
}

DECLSPEC uint64_t CreateRpcClient(Transport::TransportType transport, const char* server_addr) {
	return (uint64_t)new JsonRPCClient(transport, server_addr);
}

DECLSPEC AsyncID  AsyncRpcCall(uint64_t client_id, const char* json_call, char* json_call_result, size_t json_call_result_len, AsyncJsonReplyProcedure reply_proc) {
	auto client = (JsonRPCClient*)client_id;
	return client->AsyncRpcCall(json_call, json_call_result, json_call_result_len, reply_proc);
}

DECLSPEC uint64_t RpcCall(uint64_t client_id, const char* json_call, char* json_call_result, size_t json_call_result_len) {
	auto client = (JsonRPCClient*)client_id;
	return client->RpcCall(json_call, json_call_result, json_call_result_len);
}

DECLSPEC void  DestroyRpcClient(uint64_t client_id) {
	delete (JsonRPCClient*)client_id;
}

void JsonRPCClient::AsyncRpcReplyProc(AsyncID asyncId, unsigned long result) {
}
AsyncID	JsonRPCClient::AsyncRpcCall(const char* json_call, char* json_call_result, size_t json_call_result_len, AsyncJsonReplyProcedure replyProc) {
	// analyze the json call and build the parameters vector
	unique_ptr<vector<RemoteProcedureCall::ParameterBase*>> params = make_unique<vector<RemoteProcedureCall::ParameterBase*>>();

	// invoke the rpc client.
	return m_client->RpcCallAsync(JsonRPCClient::AsyncRpcReplyProc, "nop", params.get());
}

unsigned long JsonRPCClient::RpcCall(const char* json_call, char* json_call_result, size_t json_call_result_len) {
	unsigned long result = -1;

	// analyze the json call and build the parameters vector
	string function;
	unique_ptr<vector<RemoteProcedureCall::ParameterBase*>> params = make_unique<vector<RemoteProcedureCall::ParameterBase*>>();
	if (!BuildParametersFromJson(json_call, function, params))
		return -1;

	// invoke the rpc client.
	result = m_client->RpcCall(function, params.get());

	// build the result json.
	string json_result;
	BuildJsonFromParameters(function, params, json_result);

	// copy the result json into the result buffer if it is long enough, else, return an error.
	if (json_call_result_len > json_result.size()) {
		strncpy_s(json_call_result, json_call_result_len, json_result.c_str(), json_result.size());
	}

	return result;
}


bool JsonRPCClient::BuildParametersFromJson(const char* json_call, string& function, unique_ptr<vector<RemoteProcedureCall::ParameterBase*>>& params) {

	json call = json::parse(json_call);
	if (call.is_null() || !call.contains("function"))
		return false;

	function = call["function"];
	if (function.empty())
		return false;

	json jparams = call["parameters"];
	if (!jparams.is_null() && jparams.is_array() && !jparams.empty()) {
		for (auto param : jparams) {
			if (param["type"] == "STRING") {
				string value = param["value"];
				params->push_back(new RemoteProcedureCall::Parameter<string>(value));
			}
			else if (param["type"] == "STRING_REF") {
				string* valueP = new string(param["value"]);
				params->push_back(new RemoteProcedureCall::Parameter<RemoteProcedureCall::ParamType>(RemoteProcedureCall::ParamType::PTR));
				params->push_back(new RemoteProcedureCall::Parameter<string>(*valueP, (uint64_t)valueP));
			}
			else if (param["type"] == "BYTE") {
				uint8_t value = param["value"];
				params->push_back(new RemoteProcedureCall::Parameter<uint8_t>(value));
			}
			else if (param["type"] == "BYTE_REF") {
				uint8_t* valueP = new uint8_t(param["value"]);
				params->push_back(new RemoteProcedureCall::Parameter<RemoteProcedureCall::ParamType>(RemoteProcedureCall::ParamType::PTR));
				params->push_back(new RemoteProcedureCall::Parameter<uint8_t>(*valueP, (uint64_t)valueP));
			}
			else if (param["type"] == "CHAR") {
				char value = (uint8_t)param["value"];
				params->push_back(new RemoteProcedureCall::Parameter<char>(value));
			}
			else if (param["type"] == "CHAR_REF") {
				uint8_t* valueP = new uint8_t(param["value"]);
				params->push_back(new RemoteProcedureCall::Parameter<RemoteProcedureCall::ParamType>(RemoteProcedureCall::ParamType::PTR));
				params->push_back(new RemoteProcedureCall::Parameter<char>(*((char*)valueP), (uint64_t)valueP));
			}
			else if (param["type"] == "UINT16") {
				uint16_t value = param["value"];
				params->push_back(new RemoteProcedureCall::Parameter<uint16_t>(value));
			}
			else if (param["type"] == "UINT16_REF") {
				uint16_t* valueP = new uint16_t(param["value"]);
				params->push_back(new RemoteProcedureCall::Parameter<RemoteProcedureCall::ParamType>(RemoteProcedureCall::ParamType::PTR));
				params->push_back(new RemoteProcedureCall::Parameter<uint16_t>(*valueP, (uint64_t)valueP));
			}
			else if (param["type"] == "UINT32") {
				uint32_t value = param["value"];
				params->push_back(new RemoteProcedureCall::Parameter<uint32_t>(value));
			}
			else if (param["type"] == "UINT32_REF") {
				uint32_t* valueP = new uint32_t(param["value"]);
				params->push_back(new RemoteProcedureCall::Parameter<RemoteProcedureCall::ParamType>(RemoteProcedureCall::ParamType::PTR));
				params->push_back(new RemoteProcedureCall::Parameter<uint32_t>(*valueP, (uint64_t)valueP));
			}
			else if (param["type"] == "UINT64") {
				uint64_t value = param["value"];
				params->push_back(new RemoteProcedureCall::Parameter<uint64_t>(value));
			}
			else if (param["type"] == "UINT64_REF") {
				uint64_t* valueP = new uint64_t(param["value"]);
				params->push_back(new RemoteProcedureCall::Parameter<RemoteProcedureCall::ParamType>(RemoteProcedureCall::ParamType::PTR));
				params->push_back(new RemoteProcedureCall::Parameter<uint64_t>(*valueP, (uint64_t)valueP));
			}
			else if (param["type"] == "INT16") {
				int16_t value = param["value"];
				params->push_back(new RemoteProcedureCall::Parameter<int16_t>(value));
			}
			else if (param["type"] == "INT16_REF") {
				int16_t* valueP = new int16_t(param["value"]);
				params->push_back(new RemoteProcedureCall::Parameter<RemoteProcedureCall::ParamType>(RemoteProcedureCall::ParamType::PTR));
				params->push_back(new RemoteProcedureCall::Parameter<int16_t>(*valueP, (uint64_t)valueP));
			}
			else if (param["type"] == "INT32") {
				int32_t value = param["value"];
				params->push_back(new RemoteProcedureCall::Parameter<int32_t>(value));
			}
			else if (param["type"] == "INT32_REF") {
				int32_t* valueP = new int32_t(param["value"]);
				params->push_back(new RemoteProcedureCall::Parameter<RemoteProcedureCall::ParamType>(RemoteProcedureCall::ParamType::PTR));
				params->push_back(new RemoteProcedureCall::Parameter<int32_t>(*valueP, (uint64_t)valueP));
			}
			else if (param["type"] == "INT64") {
				int64_t value = param["value"];
				params->push_back(new RemoteProcedureCall::Parameter<int64_t>(value));
			}
			else if (param["type"] == "INT64_REF") {
				int64_t* valueP = new int64_t(param["value"]);
				params->push_back(new RemoteProcedureCall::Parameter<RemoteProcedureCall::ParamType>(RemoteProcedureCall::ParamType::PTR));
				params->push_back(new RemoteProcedureCall::Parameter<int64_t>(*valueP, (uint64_t)valueP));
			}
			else if (param["type"] == "DOUBLE") {
				double value = param["value"];
				params->push_back(new RemoteProcedureCall::Parameter<double>(value));
			}
			else if (param["type"] == "DOUBLE_REF") {
				double* valueP = new double(param["value"]);
				params->push_back(new RemoteProcedureCall::Parameter<RemoteProcedureCall::ParamType>(RemoteProcedureCall::ParamType::PTR));
				params->push_back(new RemoteProcedureCall::Parameter<double>(*valueP, (uint64_t)valueP));
			}
		}
	}

	params->push_back(new RemoteProcedureCall::Parameter<RemoteProcedureCall::ParamType>(RemoteProcedureCall::ParamType::END_OF_CALL));

	return true;
}

bool JsonRPCClient::BuildJsonFromParameters(string& function, unique_ptr<vector<RemoteProcedureCall::ParameterBase*>>& params, string& json_result) {
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
					parameters += param;
					break;

				case RemoteProcedureCall::ParamType::BYTE:
					param["type"] = "BYTE_REF";
					param["value"] = ParameterSafeCast(uint8_t, paramP)->GetReference(true);
					parameters += param;
					break;

				case RemoteProcedureCall::ParamType::CHAR:
					param["type"] = "CHAR_REF";
					param["value"] = ParameterSafeCast(uint8_t, paramP)->GetReference(true);
					parameters += param;
					break;

				case RemoteProcedureCall::ParamType::UINT16:
					param["type"] = "UINT16_REF";
					param["value"] = ParameterSafeCast(uint16_t, paramP)->GetReference(true);
					parameters += param;
					break;

				case RemoteProcedureCall::ParamType::INT16:
					param["type"] = "INT16_REF";
					param["value"] = ParameterSafeCast(int16_t, paramP)->GetReference(true);
					parameters += param;
					break;

				case RemoteProcedureCall::ParamType::UINT32:
					param["type"] = "UINT32_REF";
					param["value"] = ParameterSafeCast(uint32_t, paramP)->GetReference(true);
					parameters += param;
					break;

				case RemoteProcedureCall::ParamType::INT32:
					param["type"] = "INT32_REF";
					param["value"] = ParameterSafeCast(int32_t, paramP)->GetReference(true);
					parameters += param;
					break;

				case RemoteProcedureCall::ParamType::UINT64:
					param["type"] = "UINT64_REF";
					param["value"] = ParameterSafeCast(uint64_t, paramP)->GetReference(true);
					parameters += param;
					break;

				case RemoteProcedureCall::ParamType::INT64:
					param["type"] = "INT64_REF";
					param["value"] = ParameterSafeCast(int64_t, paramP)->GetReference(true);
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

