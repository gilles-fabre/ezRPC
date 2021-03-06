#include <iostream>
#include <string>
#include <vector>

#include <Transport.h>

#include "RemoteProcedureCall.h"
#include "RPCServer.h"
#include "RPCClient.h"

using namespace std;

RPCServer *gRpcServerP = NULL;

static unsigned long ByeBye(vector<RemoteProcedureCall::Parameter *> *v, void *user_dataP) {
	if (gRpcServerP)
		gRpcServerP->Stop();
	return (unsigned long)0;
}

static unsigned long Nop(vector<RemoteProcedureCall::Parameter *> *v, void *user_dataP) {
	return (unsigned long)0;
}

static unsigned long IncDouble(vector<RemoteProcedureCall::Parameter *> *v, void *user_dataP) {
	RemoteProcedureCall::Parameter *pReturn = (*v)[0];
	RemoteProcedureCall::Parameter *p1 = (*v)[1];
	double &d = p1->GetDoubleValue();
	cout << "value: " << d << endl;
	d += 0.1;
	cout << "inc'ed value: " << d << endl;
	return (unsigned long)0;
}

static unsigned long Increment(vector<RemoteProcedureCall::Parameter *> *v, void *user_dataP) {
	RemoteProcedureCall::Parameter *pReturn = (*v)[0];
	RemoteProcedureCall::Parameter *p1 = (*v)[1];
	int16_t &i = p1->GetInt16Value();
	cout << "value: " << i << endl;
	++i;
	cout << "inc'ed value: " << i << endl;
	return (unsigned long)i;
}

static unsigned long Concatenate(vector<RemoteProcedureCall::Parameter *> *v, void *user_dataP) {
	RemoteProcedureCall::Parameter *pReturn = (*v)[0];
	RemoteProcedureCall::Parameter *p1 = (*v)[1];
	RemoteProcedureCall::Parameter *p2 = (*v)[2];


	string &text = p1->GetStringValue();
	cout << "text: " << text << endl;
	string origin_text = text;
	cout << "origin_text: " << origin_text << endl;
	int16_t num = p2->GetInt16Value();
	cout << "num concat: " << num << endl;
	for (int i = 0; i < num; i++)
		text.append(origin_text);
	cout << "concatenated text: " << text << endl;
	return (unsigned long)text.length();
}

static unsigned long RepeatPrint(vector<RemoteProcedureCall::Parameter *> *v, void *user_dataP) {
	RemoteProcedureCall::Parameter *pReturn = (*v)[0];
	RemoteProcedureCall::Parameter *p1 = (*v)[1];
	RemoteProcedureCall::Parameter *p2 = (*v)[2];

	string text;
	text = p1->GetStringValue();
	int16_t num = p2->GetInt16Value();

	for (int i = 0; i < num; i++)
		cout << "repeat: " << text << endl;

	return num;
}

static unsigned long SumNumbers(vector<RemoteProcedureCall::Parameter *> *v, void *user_dataP) {
	RemoteProcedureCall::Parameter *pReturn = (*v)[0];
	RemoteProcedureCall::Parameter *p1 = (*v)[1];
	RemoteProcedureCall::Parameter *p2 = (*v)[2];

	int16_t num1 = p1->GetInt16Value();
	int16_t num2 = p2->GetInt16Value();

	cout << "sum: " << num1 + num2 << endl;

	return num1 + num2;
}

int main(int argc, char **argv) {
	if (argc < 4) {
		cout << "usage:" << endl;
		cout << "\ttest <tcp|file> server server_address" << endl;
		cout << "\ttest <tcp|file> client server_address function_name [repeat_count]" << endl;
		cout << "\ttest server_address must be addr:port for tcp" << endl;
		return -1;
	}

	cout << "argc :" << argc << endl;
	for (int i = 0; i < argc; i++)
		cout << "arg #" << i << ": " << argv[i] << endl;

	string proto = argv[1];
	string what = argv[2];
	string server_addr = argv[3];

	if (what == "server") {
		RPCServer server(proto == "tcp" ? Transport::TCP : Transport::FILE, server_addr);
		gRpcServerP = &server;

		server.RegisterProcedure("nop", &Nop);
		server.RegisterProcedure("inc", &Increment);
		server.RegisterProcedure("repeat", &RepeatPrint);
		server.RegisterProcedure("concat", &Concatenate);
		server.RegisterProcedure("sum", &SumNumbers);
		server.RegisterProcedure("incdouble", &IncDouble);
		server.RegisterProcedure("byebye", &ByeBye);


		server.IterateAndWait();
	} else if (what == "client"){
		RPCClient client(proto == "tcp" ? Transport::TCP : Transport::FILE, server_addr);

		if (argc < 5) {
			cout << "usage:" << endl;
			cout << "\ttest client <tcp|file> server_addr func_name" << endl;
			return -1;
		}

		string func_name = argv[4];

		unsigned long result = -1;
		if (func_name == "nop") {
			if (argc < 5) {
				cout << "usage:" << endl;
				cout << "\ttest client <tcp|file> server_addr nop" << endl;
				return -1;
			}
			int16_t repeat = argc == 6 ? atoi(argv[5]) : 1;
			for (int b = 0; b < repeat; b++) {
				result = client.RpcCall(func_name,
										RemoteProcedureCall::END_OF_CALL);
			}
			cout << "result value: " << result << endl;
		} else if (func_name == "inc") {
			if (argc < 6) {
				cout << "usage:" << endl;
				cout << "\ttest client <tcp|file> server_addr inc value" << endl;
				return -1;
			}
			int16_t repeat = argc == 7 ? atoi(argv[6]) : 1;
			int16_t i;
			for (int b = 0; b < repeat; b++) {
				i = atoi(argv[5]);
				result = client.RpcCall(func_name,
										RemoteProcedureCall::PTR,
										RemoteProcedureCall::INT16,
										&i,
										RemoteProcedureCall::END_OF_CALL);
			}
			cout << "result value: " << i << endl;
		} else if (func_name == "incdouble") {
			if (argc < 6) {
				cout << "usage:" << endl;
				cout << "\ttest client <tcp|file> server_addr incdouble float_value" << endl;
				return -1;
			}
			int16_t repeat = argc == 7 ? atoi(argv[6]) : 1;
			double d;
			for (int b = 0; b < repeat; b++) {
				d = atof(argv[5]);
				result = client.RpcCall(func_name,
										RemoteProcedureCall::PTR,
										RemoteProcedureCall::DOUBLE,
										&d,
										RemoteProcedureCall::END_OF_CALL);
			}
			cout << "result value: " << d << endl;
		} else if (func_name == "concat") {
			if (argc < 7) {
				cout << "usage:" << endl;
				cout << "\ttest client <tcp|file> server_addr concat string num_concat" << endl;
				return -1;
			}
			int16_t repeat = argc == 8 ? atoi(argv[7]) : 1;
			string p;
			for (int b = 0; b < repeat; b++) {
				p = argv[5];
				result = client.RpcCall(func_name,
										RemoteProcedureCall::PTR,
										RemoteProcedureCall::STRING,
										&p,
										RemoteProcedureCall::INT16,
										atoi(argv[6]),
										RemoteProcedureCall::END_OF_CALL);
			}
			cout << "result string: " << p << endl;
		} else if (func_name == "repeat") {
			if (argc < 7) {
				cout << "usage:" << endl;
				cout << "\ttest client <tcp|file> server_addr repeat string num_repeat" << endl;
				return -1;
			}
			int16_t repeat = argc == 8 ? atoi(argv[7]) : 1;
			string s;
			for (int b = 0; b < repeat; b++) {
				s = argv[5];
				result = client.RpcCall(func_name,
										RemoteProcedureCall::STRING,
										&s,
										RemoteProcedureCall::INT16,
										atoi(argv[6]),
										RemoteProcedureCall::END_OF_CALL);
			}
		} else if (func_name == "sum") {
			if (argc < 7) {
				cout << "usage:" << endl;
				cout << "\ttest client <tcp|file> server_addr sum number1 number2" << endl;
				return -1;
			}
			int16_t repeat = argc == 8 ? atoi(argv[7]) : 1;
			for (int b = 0; b < repeat; b++) {
				result = client.RpcCall(func_name,
										RemoteProcedureCall::INT16,
										atoi(argv[5]),
										RemoteProcedureCall::INT16,
										atoi(argv[6]),
										RemoteProcedureCall::END_OF_CALL);
			}
			cout << "result : " << result << endl;
		} else if (func_name == "byebye") {
			result = client.RpcCall(func_name, RemoteProcedureCall::END_OF_CALL);
		}

		cout << "rpc server has returned \"" << result << "\"" << endl;
	}

	return 0;
}




