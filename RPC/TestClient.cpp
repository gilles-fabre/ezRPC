#include <iostream>
#include <string>
#include <vector>

#include <RPCClient.h>

using namespace std;

string	  g_string;
Semaphore g_sem_put_str(0);
Semaphore g_sem_get_str(0);

static void GetStringAsyncReplyProc(AsyncID asyncId, unsigned long result) {
	cout << "asyncId : " << asyncId << ", returned and string is : " << g_string << ", result : " << result << endl;
	g_sem_get_str.R();
}

static void PutStringAsyncReplyProc(AsyncID asyncId, unsigned long result) {
	cout << "asyncId : " << asyncId << ", returned and string is : " << g_string << ", result : " << result << endl;
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
	string server_addr = argv[2];
	string func_name = argv[3];

	RPCClient client(proto == "tcp" ? Transport::TCP : Transport::FILE, server_addr);

	if (func_name == "get_string") {
		if (argc < 4) {
			cout << "usage:" << endl;
			cout << "\ttest <tcp|file> server_addr get_string [repeat_count]" << endl;
			return -1;
		}
		int16_t repeat = argc == 5 ? atoi(argv[4]) : 1;
		for (int b = 0; b < repeat; b++) {
			g_string = "overwrite me!";
			AsyncID id = client.RpcCallAsync(GetStringAsyncReplyProc,
								func_name,
								RemoteProcedureCall::PTR,
								RemoteProcedureCall::STRING,
								&g_string,
								RemoteProcedureCall::END_OF_CALL);
			cout << "\ttest get_string asyncId : " << id << endl;
			g_sem_get_str.A();
		}
	}
	else if (func_name == "put_string") {
		if (argc < 5) {
			cout << "usage:" << endl;
			cout << "\ttest <tcp|file> server_addr put_string text [repeat_count]" << endl;
			return -1;
		}
		g_string = argv[4];
		int16_t repeat = argc == 6 ? atoi(argv[5]) : 1;
		for (int b = 0; b < repeat; b++) {
			AsyncID id = client.RpcCallAsync(PutStringAsyncReplyProc,
								func_name,
								RemoteProcedureCall::PTR,
								RemoteProcedureCall::STRING,
								&g_string,
								RemoteProcedureCall::END_OF_CALL);
			cout << "\ttest client put_string asyncId : " << id << endl;
			g_sem_put_str.A();
		}
	}
	else if (func_name == "nop") {
		if (argc < 4) {
			cout << "usage:" << endl;
			cout << "\ttest <tcp|file> server_addr nop" << endl;
			return -1;
		}
		int16_t repeat = argc == 5 ? atoi(argv[4]) : 1;
		for (int b = 0; b < repeat; b++) {
			result = client.RpcCall(func_name,
									RemoteProcedureCall::END_OF_CALL);
		}
		cout << "result value: " << result << endl;
	} else if (func_name == "inc") {
		if (argc < 5) {
			cout << "usage:" << endl;
			cout << "\ttest <tcp|file> server_addr inc value" << endl;
			return -1;
		}
		int16_t repeat = argc == 6 ? atoi(argv[5]) : 1;
		int16_t i;
		for (int b = 0; b < repeat; b++) {
			i = atoi(argv[4]);
			result = client.RpcCall(func_name,
									RemoteProcedureCall::PTR,
									RemoteProcedureCall::INT16,
									&i,
									RemoteProcedureCall::END_OF_CALL);
		}
		cout << "result value: " << i << endl;
	} else if (func_name == "inc_double") {
		if (argc < 5) {
			cout << "usage:" << endl;
			cout << "\ttest <tcp|file> server_addr inc_double float_value" << endl;
			return -1;
		}
		int16_t repeat = argc == 6 ? atoi(argv[5]) : 1;
		double d;
		for (int b = 0; b < repeat; b++) {
			d = atof(argv[4]);
			result = client.RpcCall(func_name,
									RemoteProcedureCall::PTR,
									RemoteProcedureCall::DOUBLE,
									&d,
									RemoteProcedureCall::END_OF_CALL);
		}
		cout << "result value: " << d << endl;
	} else if (func_name == "concat") {
		if (argc < 6) {
			cout << "usage:" << endl;
			cout << "\ttest <tcp|file> server_addr concat string num_concat" << endl;
			return -1;
		}
		int16_t repeat = argc == 7 ? atoi(argv[6]) : 1;
		string p;
		for (int b = 0; b < repeat; b++) {
			p = argv[4];
			result = client.RpcCall(func_name,
									RemoteProcedureCall::PTR,
									RemoteProcedureCall::STRING,
									&p,
									RemoteProcedureCall::INT16,
									atoi(argv[5]),
									RemoteProcedureCall::END_OF_CALL);
		}
		cout << "result string: " << p << endl;
	} else if (func_name == "repeat") {
		if (argc < 6) {
			cout << "usage:" << endl;
			cout << "\ttest <tcp|file> server_addr repeat string num_repeat" << endl;
			return -1;
		}
		int16_t repeat = argc == 7 ? atoi(argv[6]) : 1;
		string s;
		for (int b = 0; b < repeat; b++) {
			s = argv[4];
			result = client.RpcCall(func_name,
									RemoteProcedureCall::STRING,
									&s,
									RemoteProcedureCall::INT16,
									atoi(argv[5]),
									RemoteProcedureCall::END_OF_CALL);
		}
	} else if (func_name == "sum") {
		if (argc < 6) {
			cout << "usage:" << endl;
			cout << "\ttest <tcp|file> server_addr sum number1 number2" << endl;
			return -1;
		}
		int16_t repeat = argc == 7 ? atoi(argv[6]) : 1;
		for (int b = 0; b < repeat; b++) {
			result = client.RpcCall(func_name,
									RemoteProcedureCall::INT16,
									atoi(argv[4]),
									RemoteProcedureCall::INT16,
									atoi(argv[5]),
									RemoteProcedureCall::END_OF_CALL);
		}
		cout << "result : " << result << endl;
	} else if (func_name == "byebye") {
		result = client.RpcCall(func_name, RemoteProcedureCall::END_OF_CALL);
	}

	cout << "rpc server has returned \"" << result << "\"" << endl;

	return 0;
}




