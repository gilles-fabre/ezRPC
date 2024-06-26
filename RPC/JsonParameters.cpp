#include "nlohmann/json.hpp"

#include "JsonParameters.h"

using nlohmann::json;

/*
* \brief Builds a parameters vector out of Json serialized form of an RPC call. This function can
*		 be called either on the client side (JsonRCPClient) or on the server side (JsonRPCServer).
*		 when called from a straight RPCClient, the "reference" parameter conveys the client side
*		 pointer to the variable associated with the parameter. 
* 
* \param jsonCallP is the well formed Json serialization form of the RPC Call.
* \param function is the name of the called remote procedure.
* \param params receives the extracted call parameters from the Json.
* 
* \return true is everything went fine, false else.
*/
bool JsonParameters::BuildParametersFromJson(const char* jsonCallP, string& function, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>>& params) {
	json call = json::parse(jsonCallP);
	if (call.is_null() || !call.contains("function"))
		return false;

	function = call["function"];
	if (function.empty())
		return false;

#ifdef JSONPARAMETERS_TRACES
	LogVText(JSONPARAMETERS_MODULE, 0, true, ">>> Building Parameters from Json %s", jsonCallP);
#endif

	try {
		json jparams = call["parameters"];
		if (!jparams.is_null() && jparams.is_array() && !jparams.empty()) {
			for (auto param : jparams) {
				switch (RemoteProcedureCall::GetParameterType(param["type"])) {
					case RemoteProcedureCall::STRING: {
						string value = param["value"];
						if (param["reference"] != nullptr)
							params->push_back(new RemoteProcedureCall::Parameter<string>(value, param["reference"]));
						else
							params->push_back(new RemoteProcedureCall::Parameter<string>(value));
					}
					break;

					case RemoteProcedureCall::STRING_REF: {
						string* valueP = new string(param["value"]);
						params->push_back(new RemoteProcedureCall::Parameter<RemoteProcedureCall::ParamType>(RemoteProcedureCall::ParamType::PTR));
						params->push_back(new RemoteProcedureCall::Parameter<string>(*valueP, (uint64_t)valueP));
					}
					break;

					case RemoteProcedureCall::BYTE: {
						uint8_t value = param["value"];
						if (param["reference"] != nullptr)
							params->push_back(new RemoteProcedureCall::Parameter<uint8_t>(value, param["reference"]));
						else
							params->push_back(new RemoteProcedureCall::Parameter<uint8_t>(value));
					}
					break;

					case RemoteProcedureCall::BYTE_REF: {
						uint8_t* valueP = new uint8_t(param["value"]);
						params->push_back(new RemoteProcedureCall::Parameter<RemoteProcedureCall::ParamType>(RemoteProcedureCall::ParamType::PTR));
						params->push_back(new RemoteProcedureCall::Parameter<uint8_t>(*valueP, (uint64_t)valueP));
					}
					break;

					case RemoteProcedureCall::CHAR: {
						char value = (uint8_t)param["value"];
						if (param["reference"] != nullptr)
							params->push_back(new RemoteProcedureCall::Parameter<char>(value, param["reference"]));
						else
							params->push_back(new RemoteProcedureCall::Parameter<char>(value));
					}
					break;

					case RemoteProcedureCall::CHAR_REF: {
						uint8_t* valueP = new uint8_t(param["value"]);
						params->push_back(new RemoteProcedureCall::Parameter<RemoteProcedureCall::ParamType>(RemoteProcedureCall::ParamType::PTR));
						params->push_back(new RemoteProcedureCall::Parameter<char>(*((char*)valueP), (uint64_t)valueP));
					}
					break;

					case RemoteProcedureCall::UINT16: {
						uint16_t value = param["value"];
						if (param["reference"] != nullptr)
							params->push_back(new RemoteProcedureCall::Parameter<uint16_t>(value, param["reference"]));
						else
							params->push_back(new RemoteProcedureCall::Parameter<uint16_t>(value));
					}
					break;

					case RemoteProcedureCall::UINT16_REF: {
						uint16_t* valueP = new uint16_t(param["value"]);
						params->push_back(new RemoteProcedureCall::Parameter<RemoteProcedureCall::ParamType>(RemoteProcedureCall::ParamType::PTR));
						params->push_back(new RemoteProcedureCall::Parameter<uint16_t>(*valueP, (uint64_t)valueP));
					}
					break;

					case RemoteProcedureCall::UINT32: {
						uint32_t value = param["value"];
						if (param["reference"] != nullptr)
							params->push_back(new RemoteProcedureCall::Parameter<uint32_t>(value, param["reference"]));
						else
							params->push_back(new RemoteProcedureCall::Parameter<uint32_t>(value));
					}
					break;

					case RemoteProcedureCall::UINT32_REF: {
						uint32_t* valueP = new uint32_t(param["value"]);
						params->push_back(new RemoteProcedureCall::Parameter<RemoteProcedureCall::ParamType>(RemoteProcedureCall::ParamType::PTR));
						params->push_back(new RemoteProcedureCall::Parameter<uint32_t>(*valueP, (uint64_t)valueP));
					}
					break;

					case RemoteProcedureCall::UINT64: {
						uint64_t value = param["value"];
						if (param["reference"] != nullptr)
							params->push_back(new RemoteProcedureCall::Parameter<uint64_t>(value, param["reference"]));
						else
							params->push_back(new RemoteProcedureCall::Parameter<uint64_t>(value));
					}
					break;

					case RemoteProcedureCall::UINT64_REF: {
						uint64_t* valueP = new uint64_t(param["value"]);
						params->push_back(new RemoteProcedureCall::Parameter<RemoteProcedureCall::ParamType>(RemoteProcedureCall::ParamType::PTR));
						params->push_back(new RemoteProcedureCall::Parameter<uint64_t>(*valueP, (uint64_t)valueP));
					}
					break;

					case RemoteProcedureCall::INT16: {
						int16_t value = param["value"];
						if (param["reference"] != nullptr)
							params->push_back(new RemoteProcedureCall::Parameter<int16_t>(value, param["reference"]));
						else
							params->push_back(new RemoteProcedureCall::Parameter<int16_t>(value));
					}
					break;

					case RemoteProcedureCall::INT16_REF: {
						int16_t* valueP = new int16_t(param["value"]);
						params->push_back(new RemoteProcedureCall::Parameter<RemoteProcedureCall::ParamType>(RemoteProcedureCall::ParamType::PTR));
						params->push_back(new RemoteProcedureCall::Parameter<int16_t>(*valueP, (uint64_t)valueP));
					}
				   break;

					case RemoteProcedureCall::INT32: {
						int32_t value = param["value"];
						if (param["reference"] != nullptr)
							params->push_back(new RemoteProcedureCall::Parameter<int32_t>(value, param["reference"]));
						else
							params->push_back(new RemoteProcedureCall::Parameter<int32_t>(value));
					}
				   break;

					case RemoteProcedureCall::INT32_REF: {
						int32_t* valueP = new int32_t(param["value"]);
						params->push_back(new RemoteProcedureCall::Parameter<RemoteProcedureCall::ParamType>(RemoteProcedureCall::ParamType::PTR));
						params->push_back(new RemoteProcedureCall::Parameter<int32_t>(*valueP, (uint64_t)valueP));
					}
				   break;

					case RemoteProcedureCall::INT64: {
						int64_t value = param["value"];
						if (param["reference"] != nullptr)
							params->push_back(new RemoteProcedureCall::Parameter<int64_t>(value, param["reference"]));
						else
							params->push_back(new RemoteProcedureCall::Parameter<int64_t>(value));
					}
				   break;

					case RemoteProcedureCall::INT64_REF: {
						int64_t* valueP = new int64_t(param["value"]);
						params->push_back(new RemoteProcedureCall::Parameter<RemoteProcedureCall::ParamType>(RemoteProcedureCall::ParamType::PTR));
						params->push_back(new RemoteProcedureCall::Parameter<int64_t>(*valueP, (uint64_t)valueP));
					}
				   break;

					case RemoteProcedureCall::DOUBLE: {
						double value = param["value"];
						if (param["reference"] != nullptr)
							params->push_back(new RemoteProcedureCall::Parameter<double>(value, param["reference"]));
						else
							params->push_back(new RemoteProcedureCall::Parameter<double>(value));
					}
					break;

					case RemoteProcedureCall::DOUBLE_REF: {
						double* valueP = new double(param["value"]);
						params->push_back(new RemoteProcedureCall::Parameter<RemoteProcedureCall::ParamType>(RemoteProcedureCall::ParamType::PTR));
						params->push_back(new RemoteProcedureCall::Parameter<double>(*valueP, (uint64_t)valueP));
					}
					break;
				}
			}
		}

		params->push_back(new RemoteProcedureCall::Parameter<RemoteProcedureCall::ParamType>(RemoteProcedureCall::ParamType::END_OF_CALL));
	}
	catch (exception& e) {
		cerr << "BuildParametersFromJson - exception caught : " << e.what() << endl;
		return false;
	}

	return true;
}

/*
* \brief Builds a Json serialized form of an RPC call result out of a call result parameters vector. Only ptr
*		 values are serialized.
*
* \param jsonResult is the well formed Json serialization form of the RPC Call result.
* \param function is the name of the returning remote procedure.
* \param params contains the resulting call parameters.
*
* \return true is everything went fine, false else.
*/
bool JsonParameters::BuildJsonFromResultParameters(const string& function, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>>& params, string& jsonResult) {
	try {
		// parse the parameters vector 'back' and build the result json
		json jsonReply;
		jsonReply["function"] = function;

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

						case RemoteProcedureCall::ParamType::DOUBLE:
							param["type"] = "DOUBLE_REF";
							param["value"] = ParameterSafeCast(double, paramP)->GetReference(true);
							parameters += param;
						break;
					}
				}
			}
		}

		jsonReply["parameters"] = parameters;

		jsonResult = jsonReply.dump();
	}
	catch (exception& e) {
		cerr << "BuildJsonFromResultParameters - exception caught : " << e.what() << endl;
		return false;
	}

#ifdef JSONPARAMETERS_TRACES
	LogVText(JSONPARAMETERS_MODULE, 0, true, "<<< From Result Parameters built Json %s", jsonResult.c_str());
#endif

	return true;
}

/*
* \brief Builds a Json serialized form of an RPC call out of a call parameters vector. All parameters are serialized.
*
* \param jsonResult is the well formed Json serialization form of the RPC call.
* \param function is the name of the called remote procedure.
* \param params contains the call parameters.
*
* \return true is everything went fine, false else.
*/
bool JsonParameters::BuildJsonFromCallParameters(const string& function, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>>& params, string& jsonResult) {
	try {
		// parse the parameters vector 'back' and build the result json
		json jsonReply;
		jsonReply["function"] = function;

		json parameters = json::array();

		if (params->size() >= 1) { // we don't process the END_OF_CALL parameter
			int num_params = (int)params->size();
			for (int i = 0; i < num_params; i++) {
				RemoteProcedureCall::ParameterBase* paramP = params->at(i);
				json param;

				param["reference"] = paramP->GetCallerPointer(); // if called from straight C++ (no json rpc on the client side), keep the origin ptr
				switch (paramP->GetType()) {
					case RemoteProcedureCall::ParamType::STRING_REF:
						param["type"] = "STRING_REF";
						param["value"] = ParameterSafeCast(string, paramP)->GetReference(false);
						parameters += param;
					break;

					case RemoteProcedureCall::ParamType::BYTE_REF:
						param["type"] = "BYTE_REF";
						param["value"] = ParameterSafeCast(uint8_t, paramP)->GetReference(false);
						parameters += param;
					break;

					case RemoteProcedureCall::ParamType::CHAR_REF:
						param["type"] = "CHAR_REF";
						param["value"] = ParameterSafeCast(uint8_t, paramP)->GetReference(false);
						parameters += param;
					break;

					case RemoteProcedureCall::ParamType::UINT16_REF:
						param["type"] = "UINT16_REF";
						param["value"] = ParameterSafeCast(uint16_t, paramP)->GetReference(false);
						parameters += param;
					break;

					case RemoteProcedureCall::ParamType::INT16_REF:
						param["type"] = "INT16_REF";
						param["value"] = ParameterSafeCast(int16_t, paramP)->GetReference(false);
						parameters += param;
					break;

					case RemoteProcedureCall::ParamType::UINT32_REF:
						param["type"] = "UINT32_REF";
						param["value"] = ParameterSafeCast(uint32_t, paramP)->GetReference(false);
						parameters += param;
					break;

					case RemoteProcedureCall::ParamType::INT32_REF:
						param["type"] = "INT32_REF";
						param["value"] = ParameterSafeCast(int32_t, paramP)->GetReference(false);
						parameters += param;
					break;

					case RemoteProcedureCall::ParamType::UINT64_REF:
						param["type"] = "UINT64_REF";
						param["value"] = ParameterSafeCast(uint64_t, paramP)->GetReference(false);
						parameters += param;
					break;

					case RemoteProcedureCall::ParamType::INT64_REF:
						param["type"] = "INT64_REF";
						param["value"] = ParameterSafeCast(int64_t, paramP)->GetReference(false);
						parameters += param;
					break;

					case RemoteProcedureCall::ParamType::DOUBLE_REF:
						param["type"] = "DOUBLE_REF";
						param["value"] = ParameterSafeCast(double, paramP)->GetReference(false);
						parameters += param;
					break;

					case RemoteProcedureCall::ParamType::STRING:
						param["type"] = "STRING";
						param["value"] = ParameterSafeCast(string, paramP)->GetReference(false);
						parameters += param;
					break;

					case RemoteProcedureCall::ParamType::BYTE:
						param["type"] = "BYTE";
						param["value"] = ParameterSafeCast(uint8_t, paramP)->GetReference(false);
						parameters += param;
					break;

					case RemoteProcedureCall::ParamType::CHAR:
						param["type"] = "CHAR";
						param["value"] = ParameterSafeCast(uint8_t, paramP)->GetReference(false);
						parameters += param;
					break;

					case RemoteProcedureCall::ParamType::UINT16:
						param["type"] = "UINT16";
						param["value"] = ParameterSafeCast(uint16_t, paramP)->GetReference(false);
						parameters += param;
					break;

					case RemoteProcedureCall::ParamType::INT16:
						param["type"] = "INT16";
						param["value"] = ParameterSafeCast(int16_t, paramP)->GetReference(false);
						parameters += param;
					break;

					case RemoteProcedureCall::ParamType::UINT32:
						param["type"] = "UINT32";
						param["value"] = ParameterSafeCast(uint32_t, paramP)->GetReference(false);
						parameters += param;
					break;

					case RemoteProcedureCall::ParamType::INT32:
						param["type"] = "INT32";
						param["value"] = ParameterSafeCast(int32_t, paramP)->GetReference(false);
						parameters += param;
					break;

					case RemoteProcedureCall::ParamType::UINT64:
						param["type"] = "UINT64";
						param["value"] = ParameterSafeCast(uint64_t, paramP)->GetReference(false);
						parameters += param;
					break;

					case RemoteProcedureCall::ParamType::INT64:
						param["type"] = "INT64";
						param["value"] = ParameterSafeCast(int64_t, paramP)->GetReference(false);
						parameters += param;
					break;

					case RemoteProcedureCall::ParamType::DOUBLE:
						param["type"] = "DOUBLE";
						param["value"] = ParameterSafeCast(double, paramP)->GetReference(false);
						parameters += param;
					break;
				}
			}
		}

		jsonReply["parameters"] = parameters;

		jsonResult = jsonReply.dump();
	}
	catch (exception& e) {
		cerr << "BuildJsonFromCallParameters - exception caught : " << e.what() << endl;
		return false;
	}

#ifdef JSONPARAMETERS_TRACES
	LogVText(JSONPARAMETERS_MODULE, 0, true, "<<< From Call Parameters built Json %s", jsonResult.c_str());
#endif

	return true;
}

void JsonParameters::CleanupParameters(shared_ptr<vector<RemoteProcedureCall::ParameterBase*>>& params) {
	for (RemoteProcedureCall::ParameterBase* p : *params) 
		delete p;

	params->clear();
	params.reset();
}
