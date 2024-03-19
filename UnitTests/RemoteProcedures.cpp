#include "pch.h"

#include <nlohmann/json.hpp>

#include <RemoteProcedureCall.h>

RpcReturnValue Increment(AsyncID asyncId, const string& name, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>> v, void* user_dataP) {
	RpcReturnValue r;

	if (v->size() < 2) {
		r = { RemoteProcedureErrors::ErrorCode::WrongNumberOfArguments };
		return r;
	}

	RemoteProcedureCall::Parameter<uint64_t>* pReturn = ParameterSafeCast(uint64_t, (*v)[0]);
	RemoteProcedureCall::Parameter<int16_t>* p1 = ParameterSafeCast(int16_t, (*v)[1]);


	if (!pReturn || !p1 || p1->GetCallerPointer() == NULL) {
		r = RpcReturnValue{ RemoteProcedureErrors::ErrorCode::NullPointer };
		return r;
	}

	int16_t& i = p1->GetReference();
	++i;

	r = { (uint32_t)i };
	return r;
}

RpcReturnValue IncDouble(AsyncID asyncId, const string& name, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>> v, void* user_dataP) {
	RpcReturnValue r;

	if (v->size() < 2) {
		r = RpcReturnValue{ RemoteProcedureErrors::ErrorCode::WrongNumberOfArguments };
		return r;
	}

	RemoteProcedureCall::Parameter<uint64_t>* pReturn = ParameterSafeCast(uint64_t, (*v)[0]);
	RemoteProcedureCall::Parameter<double>* p1 = ParameterSafeCast(double, (*v)[1]);

	if (!pReturn || !p1 || p1->GetCallerPointer() == NULL) {
		r = RpcReturnValue{ RemoteProcedureErrors::ErrorCode::BadArgument };
		return r;
	}

	double& d = p1->GetReference();
	cout << "value: " << d << endl;
	d += 0.1;
	cout << "inc'ed value: " << d << endl;

	r = { 0 };
	return r;
}

AsyncID g_cancelId = 0;
RpcReturnValue CancelProcedure(AsyncID asyncId, const string& name, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>> v, void* user_dataP) {
	RpcReturnValue r((uint32_t)0);

	if (v->size() < 2) {
		r = RpcReturnValue{ RemoteProcedureErrors::ErrorCode::WrongNumberOfArguments };
		return r;
	}

	RemoteProcedureCall::Parameter<uint64_t>* pReturn = ParameterSafeCast(uint64_t, (*v)[0]);
	RemoteProcedureCall::Parameter<uint64_t>* p1 = ParameterSafeCast(uint64_t, (*v)[1]);

	if (!pReturn || !p1) {
		r = RpcReturnValue{ RemoteProcedureErrors::ErrorCode::BadArgument };
		return r;
	}

	g_cancelId = (AsyncID)p1->GetReference();

	r = { 0 };
	return r;
}

RpcReturnValue CancellableProcedure(AsyncID asyncId, const string& name, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>> v, void* user_dataP) {
	RpcReturnValue r((uint32_t)0);

	while (g_cancelId != asyncId) 
		std::this_thread::sleep_for(10ms);


	r = { 0 };
	return r;
}

RpcReturnValue Nop(AsyncID asyncId, const string& name, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>> v, void* user_dataP) {
	RpcReturnValue r((uint32_t)0);

	return r;
}

RpcReturnValue SumNumbers(AsyncID asyncId, const string& name, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>> v, void* user_dataP) {
	RpcReturnValue r;

	if (v->size() < 3) {
		r = RpcReturnValue{ RemoteProcedureErrors::ErrorCode::WrongNumberOfArguments };
		return r;
	}

	RemoteProcedureCall::Parameter<uint64_t>* pReturn = ParameterSafeCast(uint64_t, (*v)[0]);
	RemoteProcedureCall::Parameter<int16_t>* p1 = ParameterSafeCast(int16_t, (*v)[1]);
	RemoteProcedureCall::Parameter<int16_t>* p2 = ParameterSafeCast(int16_t, (*v)[2]);

	if (!pReturn || !p1 || !p2) {
		r = RpcReturnValue{ RemoteProcedureErrors::ErrorCode::BadArgument };
		return r;
	}

	int16_t num1 = p1->GetReference();
	int16_t num2 = p2->GetReference();

	r = { (uint32_t)(num1 + num2) };
	return r;
}

RpcReturnValue Concatenate(AsyncID asyncId, const string& name, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>> v, void* user_dataP) {
	RpcReturnValue r;

	if (v->size() < 3) {
		r = RpcReturnValue{ RemoteProcedureErrors::ErrorCode::WrongNumberOfArguments };
		return r;
	}

	RemoteProcedureCall::Parameter<uint64_t>* pReturn = ParameterSafeCast(uint64_t, (*v)[0]);
	RemoteProcedureCall::Parameter<string>* p1 = ParameterSafeCast(string, (*v)[1]);
	RemoteProcedureCall::Parameter<int16_t>* p2 = ParameterSafeCast(int16_t, (*v)[2]);

	if (!pReturn || !p1 || !p2 || p1->GetCallerPointer() == NULL) {
		r = RpcReturnValue{ RemoteProcedureErrors::ErrorCode::BadArgument };
		return r;
	}

	string& text = p1->GetReference();
	string origin_text = text;
	int16_t num = p2->GetReference();
	for (int i = 0; i < num; i++)
		text.append(origin_text);


	r = { (uint32_t)text.length() };
	return r;
}

using namespace std;
using json = nlohmann::json;

uint64_t JsonNop(AsyncID asyncId, const char* jsonCallP, char* jsonCallResultP, size_t jsonCallResultLen) {
	RpcReturnValue r = RpcReturnValue{ 0, RemoteProcedureErrors::ErrorCode::None };

	json call = json::parse(jsonCallP);

	string jsonCallResult = call.dump();
	if (jsonCallResult.length() < jsonCallResultLen) {
		memcpy(jsonCallResultP, jsonCallResult.c_str(), jsonCallResult.length() + 1);
		r = RpcReturnValue{ 0, RemoteProcedureErrors::ErrorCode::None };
	}
	else
		r = RpcReturnValue{ 0, RemoteProcedureErrors::ErrorCode::JsonResultBufferTooSmall };

	return (uint64_t)r;
}

uint64_t JsonIncDouble(AsyncID asyncId, const char* jsonCallP, char* jsonCallResultP, size_t jsonCallResultLen) {
	RpcReturnValue r;

	try {
		json call = json::parse(jsonCallP);

		if (!call.contains("parameters") || !call["parameters"].is_array()) {
			r = RpcReturnValue{ RemoteProcedureErrors::ErrorCode::BadArgument };
			return (uint64_t)r;
		}

		if (call["parameters"].size() != 2) {
			r = RpcReturnValue{ RemoteProcedureErrors::ErrorCode::WrongNumberOfArguments };
			return (uint64_t)r;
		}

		double value = call["parameters"][1]["value"];
		value += 0.1;
		call["parameters"][1]["value"] = value;

		string jsonCallResult = call.dump();
		if (jsonCallResult.length() < jsonCallResultLen) {
			memcpy(jsonCallResultP, jsonCallResult.c_str(), jsonCallResult.length() + 1);
			r = RpcReturnValue{ 0, RemoteProcedureErrors::ErrorCode::None };
		}
		else
			r = RpcReturnValue{ 0, RemoteProcedureErrors::ErrorCode::JsonResultBufferTooSmall };
	}
	catch (...) {
		r = RpcReturnValue{ RemoteProcedureErrors::ErrorCode::BadArgument };
	}

	return (uint64_t)r;
}

uint64_t JsonIncrement(AsyncID asyncId, const char* jsonCallP, char* jsonCallResultP, size_t jsonCallResultLen) {
	RpcReturnValue r;

	try {
		json call = json::parse(jsonCallP);

		if (!call.contains("parameters") || !call["parameters"].is_array()) {
			r = RpcReturnValue{ RemoteProcedureErrors::ErrorCode::BadArgument };
			return (uint64_t)r;
		}

		if (call["parameters"].size() != 2) {
			r = RpcReturnValue{ RemoteProcedureErrors::ErrorCode::WrongNumberOfArguments };
			return (uint64_t)r;
		}

		int16_t value = call["parameters"][1]["value"];
		value++;
		call["parameters"][1]["value"] = value;

		string jsonCallResult = call.dump();
		if (jsonCallResult.length() < jsonCallResultLen) {
			memcpy(jsonCallResultP, jsonCallResult.c_str(), jsonCallResult.length() + 1);
			r = RpcReturnValue{ uint32_t(value), RemoteProcedureErrors::ErrorCode::None };
		}
		else
			r = RpcReturnValue{ 0, RemoteProcedureErrors::ErrorCode::JsonResultBufferTooSmall };
	}
	catch (...) {
		r = RpcReturnValue{ RemoteProcedureErrors::ErrorCode::BadArgument };
	}

	return (uint64_t)r;
}

uint64_t JsonConcatenate(AsyncID asyncId, const char* jsonCallP, char* jsonCallResultP, size_t jsonCallResultLen) {
	RpcReturnValue r;

	if (!jsonCallP || !*jsonCallP) {
		r = RpcReturnValue{ RemoteProcedureErrors::ErrorCode::BadArgument };
		return (uint64_t)r;
	}

	try {
		json call = json::parse(jsonCallP);

		if (!call.contains("parameters") || !call["parameters"].is_array()) {
			r = RpcReturnValue{ RemoteProcedureErrors::ErrorCode::BadArgument };
			return (uint64_t)r;
		}

		if (call["parameters"].size() != 3) {
			r = RpcReturnValue{ RemoteProcedureErrors::ErrorCode::WrongNumberOfArguments };
			return (uint64_t)r;
		}

		string text = call["parameters"][1]["value"];
		int16_t num = call["parameters"][2]["value"];

		string concatText = text;
		for (int i = 0; i < num; i++)
			concatText += text;

		call["parameters"][1]["value"] = concatText;

		string jsonCallResult = call.dump();
		if (jsonCallResult.length() < jsonCallResultLen) {
			memcpy(jsonCallResultP, jsonCallResult.c_str(), jsonCallResult.length() + 1);
			r = RpcReturnValue{ (uint32_t)concatText.length(), RemoteProcedureErrors::ErrorCode::None };
		} else
			r = RpcReturnValue{ 0, RemoteProcedureErrors::ErrorCode::JsonResultBufferTooSmall };
	} catch( ...) {
		r = RpcReturnValue{ RemoteProcedureErrors::ErrorCode::BadArgument };
	}

	return (uint64_t)r;
}

uint64_t JsonSumNumbers(AsyncID asyncId, const char* jsonCallP, char* jsonCallResultP, size_t jsonCallResultLen) {
	RpcReturnValue r;

	try {
		json call = json::parse(jsonCallP);

		if (!call.contains("parameters") || !call["parameters"].is_array()) {
			r = RpcReturnValue{ RemoteProcedureErrors::ErrorCode::BadArgument };
			return (uint64_t)r;
		}

		if (call["parameters"].size() != 3) {
			r = RpcReturnValue{ RemoteProcedureErrors::ErrorCode::WrongNumberOfArguments };
			return (uint64_t)r;
		}

		int16_t num1 = call["parameters"][1]["value"];
		int16_t num2 = call["parameters"][2]["value"];

		string jsonCallResult = call.dump();
		if (jsonCallResult.length() < jsonCallResultLen) {
			memcpy(jsonCallResultP, jsonCallResult.c_str(), jsonCallResult.length() + 1);
			r = RpcReturnValue{ uint32_t(num1 + num2), RemoteProcedureErrors::ErrorCode::None };
		}
		else
			r = RpcReturnValue{ 0, RemoteProcedureErrors::ErrorCode::JsonResultBufferTooSmall };
	}
	catch (...) {
		r = RpcReturnValue{ RemoteProcedureErrors::ErrorCode::BadArgument };
	}

	return (uint64_t)r;
}
