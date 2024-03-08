#include <iostream>
#include <string>
#include <vector>

#include <RPCServer.h>


using namespace std;

RPCServer* s_rpcServerP = NULL;

static unsigned long ByeBye(string& name, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>> v, void* user_dataP) {
	if (s_rpcServerP)
		s_rpcServerP->Stop();

	return (unsigned long)0;
}

static unsigned long Nop(string& name, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>> v, void* user_dataP) {
	return (unsigned long)0;
}

static unsigned long IncDouble(string& name, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>> v, void* user_dataP) {
	if (v->size() < 2)
		return -1;

	RemoteProcedureCall::Parameter<uint64_t>* pReturn = ParameterSafeCast(uint64_t, (*v)[0]);
	RemoteProcedureCall::Parameter<double>* p1 = ParameterSafeCast(double, (*v)[1]);

	if (!pReturn || !p1)
		return -1;

	double& d = p1->GetReference();
	cout << "value: " << d << endl;
	d += 0.1;
	cout << "inc'ed value: " << d << endl;

	return (unsigned long)0;
}

static unsigned long Increment(string& name, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>> v, void* user_dataP) {
	if (v->size() < 2)
		return -1;

	RemoteProcedureCall::Parameter<uint64_t>* pReturn = ParameterSafeCast(uint64_t, (*v)[0]);
	RemoteProcedureCall::Parameter<int16_t>* p1 = ParameterSafeCast(int16_t, (*v)[1]);


	if (!pReturn || !p1)
		return -1;

	int16_t& i = p1->GetReference();
	cout << "value: " << i << endl;
	++i;
	cout << "inc'ed value: " << i << endl;

	return (unsigned long)i;
}

static unsigned long Concatenate(string& name, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>> v, void* user_dataP) {
	if (v->size() < 3)
		return -1;

	RemoteProcedureCall::Parameter<uint64_t>* pReturn = ParameterSafeCast(uint64_t, (*v)[0]);
	RemoteProcedureCall::Parameter<string>* p1 = ParameterSafeCast(string, (*v)[1]);
	RemoteProcedureCall::Parameter<int16_t>* p2 = ParameterSafeCast(int16_t, (*v)[2]);

	if (!pReturn || !p1 || !p2)
		return -1;

	string& text = p1->GetReference();
	cout << "text: " << text << endl;
	string origin_text = text;
	cout << "origin_text: " << origin_text << endl;
	int16_t num = p2->GetReference();
	cout << "num concat: " << num << endl;
	for (int i = 0; i < num; i++)
		text.append(origin_text);
	cout << "concatenated text: " << text << endl;

	return (unsigned long)text.length();
}

static unsigned long RepeatPrint(string& name, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>> v, void* user_dataP) {
	if (v->size() < 3)
		return -1;

	RemoteProcedureCall::Parameter<uint64_t>* pReturn = ParameterSafeCast(uint64_t, (*v)[0]);
	RemoteProcedureCall::Parameter<string>* p1 = ParameterSafeCast(string, (*v)[1]);
	RemoteProcedureCall::Parameter<int16_t>* p2 = ParameterSafeCast(int16_t, (*v)[2]);

	if (!pReturn || !p1 || !p2)
		return -1;

	string text;
	text = p1->GetReference();
	int16_t num = p2->GetReference();

	for (int i = 0; i < num; i++)
		cout << "repeat: " << text << endl;

	return num;
}

static unsigned long SumNumbers(string& name, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>> v, void* user_dataP) {
	if (v->size() < 3)
		return -1;

	RemoteProcedureCall::Parameter<uint64_t>* pReturn = ParameterSafeCast(uint64_t, (*v)[0]);
	RemoteProcedureCall::Parameter<int16_t>* p1 = ParameterSafeCast(int16_t, (*v)[1]);
	RemoteProcedureCall::Parameter<int16_t>* p2 = ParameterSafeCast(int16_t, (*v)[2]);

	if (!pReturn || !p1 || !p2)
		return -1;

	int16_t num1 = p1->GetReference();
	int16_t num2 = p2->GetReference();

	cout << "sum: " << num1 + num2 << endl;

	return num1 + num2;
}

static unsigned long GetString(string& name, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>> v, void* user_dataP) {
	if (v->size() < 2)
		return -1;

	RemoteProcedureCall::Parameter<uint64_t>* pReturn = ParameterSafeCast(uint64_t, (*v)[0]);
	RemoteProcedureCall::Parameter<string>* p1 = ParameterSafeCast(string, (*v)[1]);

	if (!pReturn || !p1)
		return -1;

	string& text = p1->GetReference();

	cout << "enter a string :" << endl;

	cin >> text;

	return (unsigned long)text.length();
}

static unsigned long PutString(string& name, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>> v, void* user_dataP) {
	if (v->size() < 2)
		return -1;

	RemoteProcedureCall::Parameter<uint64_t>* pReturn = ParameterSafeCast(uint64_t, (*v)[0]);
	RemoteProcedureCall::Parameter<string>* p1 = ParameterSafeCast(string, (*v)[1]);

	if (!pReturn || !p1)
		return -1;

	string& text = p1->GetReference();

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
	string serverAddr = argv[2];

	RPCServer server(proto == "tcp" ? Transport::TCP : Transport::FILE, serverAddr);
	s_rpcServerP = &server;

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




