#include <iostream>
#include <string>
#include <vector>

#include <JsonRPCClient.h>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

string	  g_string;
Semaphore g_sem_put_str(0);
Semaphore g_sem_get_str(0);

char g_Buffer[256 + 1];

static void GetStringAsyncReplyProc(AsyncID asyncId, unsigned long result) {
	cout << "asyncId : " << asyncId << ", returned json : " << g_Buffer << " and result : " << result << endl;
	g_sem_get_str.R();
}

static void PutStringAsyncReplyProc(AsyncID asyncId, unsigned long result) {
	cout << "asyncId : " << asyncId << ", returned json : " << g_Buffer << " and result : " << result << endl;
	g_sem_put_str.R();
}

int main(int argc, char **argv) {
	if (argc < 4) {
		cout << "usage:" << endl;
		cout << argv[0] << "\t <tcp|file> server_address function_name [repeat_count]" << endl;
		cout << argv[0] << "\t server_address must be addr:port for tcp" << endl;
		cout << argv[0] << "\n\tregistered funtions are: nop, inc <int>, repeat <string> <int>,\n\tconcat <string> <int>, sum <int> <int>, inc_double <double>, byebye,\n\tput_string, get_string." << endl;
		return -1;
	}

	cout << "argc :" << argc << endl;
	for (int i = 0; i < argc; i++)
		cout << "arg #" << i << ": " << argv[i] << endl;

	unsigned long result = -1;

	string proto = argv[1];
	string serverAddr = argv[2];
	string function = argv[3];

	uint64_t jclient = CreateRpcClient(proto == "tcp" ? Transport::TCP : Transport::FILE, serverAddr.c_str());

	if (function == "get_string") {
		if (argc < 4) {
			cout << "usage:" << endl;
			cout << "\ttest <tcp|file> server_addr get_string [repeat_count]" << endl;
			return -1;
		}
		int16_t repeat = argc == 5 ? atoi(argv[4]) : 1;
		for (int b = 0; b < repeat; b++) {
			json call;
			call["function"] = function;
			json parameters = json::array();
			{
				json param;
				param["type"] = "STRING_REF";
				param["value"] = argv[3];
				parameters += param;
			}
			call["parameters"] = parameters;
			RpcCallAsync(jclient, call.dump().c_str(), g_Buffer, sizeof(g_Buffer), GetStringAsyncReplyProc);
			g_sem_get_str.A();
		}
	}
	else if (function == "put_string") {
		if (argc < 5) {
			cout << "usage:" << endl;
			cout << "\ttest <tcp|file> server_addr put_string text [repeat_count]" << endl;
			return -1;
		}
		g_string = argv[4];
		int16_t repeat = argc == 6 ? atoi(argv[5]) : 1;
		for (int b = 0; b < repeat; b++) {
			json call;
			call["function"] = function;
			json parameters = json::array();
			{
				json param;
				param["type"] = "STRING";
				param["value"] = argv[4];
				parameters += param;
			}
			call["parameters"] = parameters;
			RpcCallAsync(jclient, call.dump().c_str(), g_Buffer, sizeof(g_Buffer), PutStringAsyncReplyProc);
			g_sem_put_str.A();
		}
	} 
	if (function == "nop") {
		if (argc < 4) {
			cout << "usage:" << endl;
			cout << "\ttest <tcp|file> server_addr nop" << endl;
			return -1;
		}
		int16_t repeat = argc == 5 ? atoi(argv[4]) : 1;
		for (int b = 0; b < repeat; b++) {
			json call;
			call["function"] = function;
			RpcCall(jclient, call.dump().c_str(), g_Buffer, sizeof(g_Buffer));
			cout << "returned json : " << string(g_Buffer) << endl;
		}
	} else if (function == "inc") {
		if (argc < 5) {
			cout << "usage:" << endl;
			cout << "\ttest <tcp|file> server_addr inc value" << endl;
			return -1;
		}
		int16_t repeat = argc == 6 ? atoi(argv[5]) : 1;
		for (int b = 0; b < repeat; b++) {
			json call;
			call["function"] = function;
			json parameters = json::array();
			{
				json param;
				param["type"] = "INT16_REF";
				param["value"] = std::atoi(argv[4]);
				parameters += param;
			}
			call["parameters"] = parameters;
			RpcCall(jclient, call.dump().c_str(), g_Buffer, sizeof(g_Buffer));
			cout << "returned json : " << string(g_Buffer) << endl;
		}
	} else if (function == "inc_double") {
		if (argc < 5) {
			cout << "usage:" << endl;
			cout << "\ttest <tcp|file> server_addr inc_double float_value" << endl;
			return -1;
		}
		int16_t repeat = argc == 6 ? atoi(argv[5]) : 1;
		for (int b = 0; b < repeat; b++) {
			json call;
			call["function"] = function;
			json parameters = json::array();
			{
				json param;
				param["type"] = "DOUBLE_REF";
				param["value"] = std::atof(argv[4]);
				parameters += param;
			}
			call["parameters"] = parameters;
			RpcCall(jclient, call.dump().c_str(), g_Buffer, sizeof(g_Buffer));
			cout << "returned json : " << string(g_Buffer) << endl;
		}
	} else if (function == "concat") {
		if (argc < 6) {
			cout << "usage:" << endl;
			cout << "\ttest <tcp|file> server_addr concat string num_concat" << endl;
			return -1;
		}
		int16_t repeat = argc == 7 ? atoi(argv[6]) : 1;
		for (int b = 0; b < repeat; b++) {
			json call;
			call["function"] = function;
			json parameters = json::array();
			{
				json param;
				param["type"] = "STRING_REF";
				param["value"] = argv[4];
				parameters += param;
			}
			{
				json param;
				param["type"] = "INT16";
				param["value"] = std::atoi(argv[5]);
				parameters += param;
			}
			call["parameters"] = parameters;
			RpcCall(jclient, call.dump().c_str(), g_Buffer, sizeof(g_Buffer));
			cout << "returned json : " << string(g_Buffer) << endl;
		}
	} else if (function == "repeat") {
		if (argc < 6) {
			cout << "usage:" << endl;
			cout << "\ttest <tcp|file> server_addr repeat string num_repeat" << endl;
			return -1;
		}
		int16_t repeat = argc == 7 ? atoi(argv[6]) : 1;
		for (int b = 0; b < repeat; b++) {
			json call;
			call["function"] = function;
			json parameters = json::array();
			{
				json param;
				param["type"] = "STRING";
				param["value"] = argv[4];
				parameters += param;
			}
			{
				json param;
				param["type"] = "INT16";
				param["value"] = std::atoi(argv[5]);
				parameters += param;
			}
			call["parameters"] = parameters;
			RpcCall(jclient, call.dump().c_str(), g_Buffer, sizeof(g_Buffer));
			cout << "returned json : " << string(g_Buffer) << endl;
		}
	} else if (function == "sum") {
		if (argc < 6) {
			cout << "usage:" << endl;
			cout << "\ttest <tcp|file> server_addr sum number1 number2" << endl;
			return -1;
		}
		int16_t repeat = argc == 7 ? atoi(argv[6]) : 1;
		for (int b = 0; b < repeat; b++) {
			json call;
			call["function"] = function;
			json parameters = json::array();
			{
				json param;
				param["type"] = "INT16";
				param["value"] = std::atoi(argv[4]);
				parameters += param;
			}
			{
				json param;
				param["type"] = "INT16";
				param["value"] = std::atoi(argv[5]);
				parameters += param;
			}
			call["parameters"] = parameters;
			RpcCall(jclient, call.dump().c_str(), g_Buffer, sizeof(g_Buffer));
			cout << "returned json : " << string(g_Buffer) << endl;
		}
	} else if (function == "byebye") {
		json call;
		call["function"] = function;
		RpcCall(jclient, call.dump().c_str(), g_Buffer, sizeof(g_Buffer));
	}

	cout << "rpc server has returned \"" << result << "\"" << endl;

	DestroyRpcClient(jclient);

	return 0;
}




