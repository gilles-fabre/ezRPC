#include <iostream>
#include <string>
#include <vector>

#include <RPCServer.h>


using namespace std;

RPCServer* gRpcServerP = NULL;

static unsigned long ByeBye(vector<RemoteProcedureCall::Parameter*>* v, void* user_dataP) {
	if (gRpcServerP)
		gRpcServerP->Stop();
	return (unsigned long)0;
}

static unsigned long Nop(vector<RemoteProcedureCall::Parameter*>* v, void* user_dataP) {
	return (unsigned long)0;
}

static unsigned long IncDouble(vector<RemoteProcedureCall::Parameter*>* v, void* user_dataP) {
	RemoteProcedureCall::Parameter* pReturn = (*v)[0];
	RemoteProcedureCall::Parameter* p1 = (*v)[1];
	double& d = p1->GetDoubleReference();
	cout << "value: " << d << endl;
	d += 0.1;
	cout << "inc'ed value: " << d << endl;
	return (unsigned long)0;
}

static unsigned long Increment(vector<RemoteProcedureCall::Parameter*>* v, void* user_dataP) {
	RemoteProcedureCall::Parameter* pReturn = (*v)[0];
	RemoteProcedureCall::Parameter* p1 = (*v)[1];
	int16_t& i = p1->GetInt16Reference();
	cout << "value: " << i << endl;
	++i;
	cout << "inc'ed value: " << i << endl;
	return (unsigned long)i;
}

static unsigned long Concatenate(vector<RemoteProcedureCall::Parameter*>* v, void* user_dataP) {
	RemoteProcedureCall::Parameter* pReturn = (*v)[0];
	RemoteProcedureCall::Parameter* p1 = (*v)[1];
	RemoteProcedureCall::Parameter* p2 = (*v)[2];


	string& text = p1->GetStringReference();
	cout << "text: " << text << endl;
	string origin_text = text;
	cout << "origin_text: " << origin_text << endl;
	int16_t num = p2->GetInt16Reference();
	cout << "num concat: " << num << endl;
	for (int i = 0; i < num; i++)
		text.append(origin_text);
	cout << "concatenated text: " << text << endl;
	return (unsigned long)text.length();
}

static unsigned long RepeatPrint(vector<RemoteProcedureCall::Parameter*>* v, void* user_dataP) {
	RemoteProcedureCall::Parameter* pReturn = (*v)[0];
	RemoteProcedureCall::Parameter* p1 = (*v)[1];
	RemoteProcedureCall::Parameter* p2 = (*v)[2];

	string text;
	text = p1->GetStringReference();
	int16_t num = p2->GetInt16Reference();

	for (int i = 0; i < num; i++)
		cout << "repeat: " << text << endl;

	return num;
}

static unsigned long SumNumbers(vector<RemoteProcedureCall::Parameter*>* v, void* user_dataP) {
	RemoteProcedureCall::Parameter* pReturn = (*v)[0];
	RemoteProcedureCall::Parameter* p1 = (*v)[1];
	RemoteProcedureCall::Parameter* p2 = (*v)[2];

	int16_t num1 = p1->GetInt16Reference();
	int16_t num2 = p2->GetInt16Reference();

	cout << "sum: " << num1 + num2 << endl;

	return num1 + num2;
}

static unsigned long GetString(vector<RemoteProcedureCall::Parameter*>* v, void* user_dataP) {
	RemoteProcedureCall::Parameter* pReturn = (*v)[0];
	RemoteProcedureCall::Parameter* p1 = (*v)[1];

	string& text = p1->GetStringReference();

	cout << "enter a string :" << endl;

	cin >> text;

	return (unsigned long)text.length();
}

static unsigned long PutString(vector<RemoteProcedureCall::Parameter*>* v, void* user_dataP) {
	RemoteProcedureCall::Parameter* pReturn = (*v)[0];
	RemoteProcedureCall::Parameter* p1 = (*v)[1];

	string& text = p1->GetStringReference();

	cout << "string passed :" << text << endl;

	return 0;
}

int main(int argc, char **argv) {
	if (argc < 3) {
		cout << "usage:" << endl;
		cout << argv[0] << "\t <tcp|file> server_address" << endl;
		return -1;
	}

	cout << "argc :" << argc << endl;
	for (int i = 0; i < argc; i++)
		cout << "arg #" << i << ": " << argv[i] << endl;

	string proto = argv[1];
	string server_addr = argv[2];

	RPCServer server(proto == "tcp" ? Transport::TCP : Transport::FILE, server_addr);
	gRpcServerP = &server;

	server.RegisterProcedure("nop", &Nop);
	server.RegisterProcedure("inc", &Increment);
	server.RegisterProcedure("repeat", &RepeatPrint);
	server.RegisterProcedure("concat", &Concatenate);
	server.RegisterProcedure("sum", &SumNumbers);
	server.RegisterProcedure("inc_double", &IncDouble);
	server.RegisterProcedure("byebye", &ByeBye);
	server.RegisterProcedure("get_string", &GetString);
	server.RegisterProcedure("put_string", &PutString);

	server.IterateAndWait();

	return 0;
}




