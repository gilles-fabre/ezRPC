#include <iostream>
#include <string>
#include <vector>
#include <math.h>

#include <RemoteProcedureCall.h>
#include <RPCServer.h>
#include <RPCClient.h>

using namespace std;

RPCServer 		*gRpcClientP = NULL;
RPCClient 		*gRpcStateMachineP = NULL;

static const std::string sStep("step");
static const std::string sStepBack("step_back");
static const std::string sStart("start");

static int RandomAddQuestion() {
	srand48(time(NULL));
	int a = rand() % 10;
	int b = rand() % 10;
	cout << "what's the sum of " << a << " + " << b << "?" << endl;
	return a + b;
}
static unsigned long GameStart(vector<RemoteProcedureCall::Parameter *> *v, void *user_dataP) {
	RemoteProcedureCall::Parameter *pReturn = (*v)[0];
	RemoteProcedureCall::Parameter *pMachine = (*v)[2];
	RemoteProcedureCall::Parameter *pSource = (*v)[3];
	RemoteProcedureCall::Parameter *pTransition = (*v)[4];
	RemoteProcedureCall::Parameter *pDestination = (*v)[5];

	cout << "Machine("<< pMachine->GetStringValue() << "):" << pSource->GetStringValue() << "-" << pTransition->GetStringValue() << "->" << pDestination->GetStringValue() << endl;
	cout << "Let's start playing" << endl;
	int expected = RandomAddQuestion();
	int answer;
	cin >> answer;
	if (expected == answer)
		gRpcStateMachineP->RpcCall("do_transition", RemoteProcedureCall::STRING, sStep, RemoteProcedureCall::END_OF_CALL);
	else
		gRpcStateMachineP->RpcCall("do_transition", RemoteProcedureCall::STRING, sStepBack, RemoteProcedureCall::END_OF_CALL);

	return 0;
}
static unsigned long Step(vector<RemoteProcedureCall::Parameter *> *v, void *user_dataP) {
	RemoteProcedureCall::Parameter *pReturn = (*v)[0];
	RemoteProcedureCall::Parameter *pMachine = (*v)[2];
	RemoteProcedureCall::Parameter *pSource = (*v)[3];
	RemoteProcedureCall::Parameter *pTransition = (*v)[4];
	RemoteProcedureCall::Parameter *pDestination = (*v)[5];

	cout << "Machine("<< pMachine->GetStringValue() << "):" << pSource->GetStringValue() << "-" << pTransition->GetStringValue() << "->" << pDestination->GetStringValue() << endl;
	cout << "Let's keep on playing" << endl;
	int expected = RandomAddQuestion();
	int answer;
	cin >> answer;
	if (expected == answer)
		gRpcStateMachineP->RpcCall("do_transition", RemoteProcedureCall::STRING, sStep, RemoteProcedureCall::END_OF_CALL);
	else
		gRpcStateMachineP->RpcCall("do_transition", RemoteProcedureCall::STRING, sStepBack, RemoteProcedureCall::END_OF_CALL);

	return 0;
}
static unsigned long Win(vector<RemoteProcedureCall::Parameter *> *v, void *user_dataP) {
	RemoteProcedureCall::Parameter *pReturn = (*v)[0];
	RemoteProcedureCall::Parameter *pMachine = (*v)[2];
	RemoteProcedureCall::Parameter *pSource = (*v)[3];
	RemoteProcedureCall::Parameter *pTransition = (*v)[4];
	RemoteProcedureCall::Parameter *pDestination = (*v)[5];

	cout << "Machine("<< pMachine->GetStringValue() << "):" << pSource->GetStringValue() << "-" << pTransition->GetStringValue() << "->" << pDestination->GetStringValue() << endl;
	cout << "You Won!!" << endl;
	cout << "Play a new game (Y/N)? : ";
	char  c;
	cin >> c;
	if (c == 'Y')
		gRpcStateMachineP->RpcCall("do_transition", RemoteProcedureCall::STRING, sStart, RemoteProcedureCall::END_OF_CALL);
	else exit(0);

	return 0;
}
static unsigned long GameOver(vector<RemoteProcedureCall::Parameter *> *v, void *user_dataP) {
	RemoteProcedureCall::Parameter *pReturn = (*v)[0];
	RemoteProcedureCall::Parameter *pMachine = (*v)[2];
	RemoteProcedureCall::Parameter *pSource = (*v)[3];
	RemoteProcedureCall::Parameter *pTransition = (*v)[4];
	RemoteProcedureCall::Parameter *pDestination = (*v)[5];

	cout << "Machine("<< pMachine->GetStringValue() << "):" << pSource->GetStringValue() << "-" << pTransition->GetStringValue() << "->" << pDestination->GetStringValue() << endl;
	cout << "You lost!!" << endl;
	cout << "Play a new game (Y/N)? : ";
	char  c;
	cin >> c;
	if (c == 'Y')
		gRpcStateMachineP->RpcCall("do_transition", RemoteProcedureCall::STRING, sStart, RemoteProcedureCall::END_OF_CALL);
	else exit(0);

	return 0;
}
int main(int argc, char **argv) {
	if (argc != 3) {
		cout << "usage:" << endl;
		cout << argv[0] << "\t client_address state_machine_address" << endl;
		return -1;
	}

	cout << "argc :" << argc << endl;
	for (int i = 0; i < argc; i++)
		cout << "arg #" << i << ": " << argv[i] << endl;

	string client_addr = argv[1];
	string state_machine_addr = argv[2];

	RPCServer server(Transport::TCP, client_addr);
	gRpcClientP = &server;
	
	server.RegisterProcedure("to_game_start", &GameStart);
	server.RegisterProcedure("to_one", &Step);
	server.RegisterProcedure("to_two", &Step);
	server.RegisterProcedure("to_three", &Step);
	server.RegisterProcedure("to_win", &Win);
	server.RegisterProcedure("to_game_over", &GameOver);

	RPCClient client(Transport::TCP, state_machine_addr);
	gRpcStateMachineP = &client;
	
	client.RpcCall("delayed_connect", 
				   RemoteProcedureCall::STRING, client_addr, 	// tell the state machine to connect here after one second
				   RemoteProcedureCall::END_OF_CALL); 			

	server.IterateAndWait();

	return 0;
}




