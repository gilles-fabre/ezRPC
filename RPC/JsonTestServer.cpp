#include <iostream>
#include <string>
#include <vector>

#include <JsonRPCServer.h>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

uint64_t g_jserver;

static uint64_t ByeBye(const char* jsonCallP, char* jsonCallResultP, size_t jsonCallResultLen) {
	json call = json::parse(jsonCallP);

	string jsonCallResult = call.dump();
	if (jsonCallResult.length() < jsonCallResultLen)
		memcpy(jsonCallResultP, jsonCallResult.c_str(), jsonCallResult.length() + 1);

	return 0;
}

static uint64_t Nop(const char* jsonCallP, char* jsonCallResultP, size_t jsonCallResultLen) {
	json call = json::parse(jsonCallP);

	string jsonCallResult = call.dump();
	if (jsonCallResult.length() < jsonCallResultLen)
		memcpy(jsonCallResultP, jsonCallResult.c_str(), jsonCallResult.length() + 1);

	return 0;
}

static uint64_t IncDouble(const char* jsonCallP, char* jsonCallResultP, size_t jsonCallResultLen) {
	json call = json::parse(jsonCallP);

	double value = call["parameters"][1]["value"];
	value += 0.1;
	call["parameters"][1]["value"] = value;

	string jsonCallResult = call.dump();
	if (jsonCallResult.length() < jsonCallResultLen)
		memcpy(jsonCallResultP, jsonCallResult.c_str(), jsonCallResult.length() + 1);

	return 0;
}

static uint64_t Increment(const char* jsonCallP, char* jsonCallResultP, size_t jsonCallResultLen) {
	json call = json::parse(jsonCallP);

	int16_t value = call["parameters"][1]["value"];
	value++; 
	call["parameters"][1]["value"] = value;

	string jsonCallResult = call.dump();
	if (jsonCallResult.length() < jsonCallResultLen)
		memcpy(jsonCallResultP, jsonCallResult.c_str(), jsonCallResult.length() + 1);

	return 0;
}

static uint64_t Concatenate(const char* jsonCallP, char* jsonCallResultP, size_t jsonCallResultLen) {
	json call = json::parse(jsonCallP);

	string text = call["parameters"][1]["value"];
	int16_t num = call["parameters"][2]["value"];

	string concatText = text;
	for (int i = 0; i < num; i++)
		concatText += text;

	call["parameters"][1]["value"] = concatText;

	string jsonCallResult = call.dump();
	if (jsonCallResult.length() < jsonCallResultLen)
		memcpy(jsonCallResultP, jsonCallResult.c_str(), jsonCallResult.length() + 1);

	return (uint64_t)concatText.length();
}

static uint64_t RepeatPrint(const char* jsonCallP, char* jsonCallResultP, size_t jsonCallResultLen) {
	json call = json::parse(jsonCallP);

	string text = call["parameters"][1]["value"];
	int16_t num = call["parameters"][2]["value"];

	for (int i = 0; i < num; i++)
		cout << text << endl;

	string jsonCallResult = call.dump();
	if (jsonCallResult.length() < jsonCallResultLen)
		memcpy(jsonCallResultP, jsonCallResult.c_str(), jsonCallResult.length() + 1);

	return (uint64_t)num;
}

static uint64_t SumNumbers(const char* jsonCallP, char* jsonCallResultP, size_t jsonCallResultLen) {
	json call = json::parse(jsonCallP);

	int16_t num1 = call["parameters"][1]["value"];
	int16_t num2 = call["parameters"][2]["value"];

	cout << num1 + num2 << endl;

	string jsonCallResult = call.dump();
	if (jsonCallResult.length() < jsonCallResultLen)
		memcpy(jsonCallResultP, jsonCallResult.c_str(), jsonCallResult.length() + 1);

	return (uint64_t)(num1 + num2);
}

static uint64_t GetString(const char* jsonCallP, char* jsonCallResultP, size_t jsonCallResultLen) {
	json call = json::parse(jsonCallP);

	string text = call["parameters"][1]["value"];

	cout << "enter a string: "<< endl;
	cin >> text;
	call["parameters"][1]["value"] = text;

	string jsonCallResult = call.dump();
	if (jsonCallResult.length() < jsonCallResultLen)
		memcpy(jsonCallResultP, jsonCallResult.c_str(), jsonCallResult.length() + 1);

	return (uint64_t)text.length();
}

static uint64_t PutString(const char* jsonCallP, char* jsonCallResultP, size_t jsonCallResultLen) {

	json call = json::parse(jsonCallP);

	string text = call["parameters"][1]["value"];

	cout << "string passed :" << text << endl;

	string jsonCallResult = call.dump();
	if (jsonCallResult.length() < jsonCallResultLen)
		memcpy(jsonCallResultP, jsonCallResult.c_str(), jsonCallResult.length() + 1);

	return 0;
}

int main(int argc, char** argv) {
	if (argc < 3) {
		cout << "usage:" << endl;
		cout << argv[0] << "\t <tcp|file> server_address" << endl;
		return -1;
	}

	cout << "argc :" << argc << endl;
	for (int i = 0; i < argc; i++)
		cout << "arg #" << i << ": " << argv[i] << endl;

	string proto = argv[1];
	string serverAddr = argv[2];


	g_jserver = CreateRpcServer(proto == "tcp" ? Transport::TCP : Transport::FILE, serverAddr.c_str());

	RegisterProcedure(g_jserver, "nop", &Nop);
	RegisterProcedure(g_jserver, "inc", &Increment);
	RegisterProcedure(g_jserver, "repeat", &RepeatPrint);
	RegisterProcedure(g_jserver, "concat", &Concatenate);
	RegisterProcedure(g_jserver, "sum", &SumNumbers);
	RegisterProcedure(g_jserver, "inc_double", &IncDouble);
	RegisterProcedure(g_jserver, "byebye", &ByeBye);
	RegisterProcedure(g_jserver, "get_string", &GetString);
	RegisterProcedure(g_jserver, "put_string", &PutString);

	IterateAndWait(g_jserver);

	return 0;
}




