#include <iostream>
#include <string>
#include <vector>
#include <math.h>
#include <fstream>

#include <RemoteProcedureCall.h>
#include <RPCServer.h>
#include <RPCClient.h>
#include <StateMachine.h>

using namespace std;

RPCServer 		*gRpcMachineClientP = NULL;
RPCClient 		*gRpcMachineServerP = NULL;
uint64_t 		gMachineId;

static const std::string sStep("step");
static const std::string sStepBack("step_back");
static const std::string sStart("start");

static 	Thread* gThreadP = NULL;

static int RandomAddQuestion() {
	srand48(time(NULL));
	int a = rand() % 10;
	int b = rand() % 10;
	cout << "what's the sum of " << a << " + " << b << "?" << endl;
	return a + b;
}
static void PromptTheUserAndDoTransition(void *parameterP) {
	Thread *threadP = (Thread*)parameterP;
	int expected = RandomAddQuestion();
	int answer;
	cin >> answer;
	if (!threadP->IsAskedToStop()) {
		if (expected == answer)
			gRpcMachineServerP->RpcCall("do_transition", RemoteProcedureCall::UINT64, gMachineId, RemoteProcedureCall::STRING, sStep, RemoteProcedureCall::END_OF_CALL);
		else
			gRpcMachineServerP->RpcCall("do_transition", RemoteProcedureCall::UINT64, gMachineId, RemoteProcedureCall::STRING, sStepBack, RemoteProcedureCall::END_OF_CALL);
	} else {
		cout << endl << "you're too slow dude!" << endl;
	}

	delete threadP;
}

static unsigned long GameStart(vector<RemoteProcedureCall::Parameter *> *v, void *user_dataP) {
	RemoteProcedureCall::Parameter *pReturn = (*v)[0];
	RemoteProcedureCall::Parameter *pMachine = (*v)[2];
	RemoteProcedureCall::Parameter *pSource = (*v)[3];
	RemoteProcedureCall::Parameter *pTransition = (*v)[4];
	RemoteProcedureCall::Parameter *pDestination = (*v)[5];

	cout << "Machine("<< pMachine->GetStringReference() << "):" << pSource->GetStringReference() << "-" << pTransition->GetStringReference() << "->" << pDestination->GetStringReference() << endl;
	cout << "Let's start playing" << endl;

	if (gThreadP && gThreadP->IsRunning()) 
		gThreadP->Stop();

	gThreadP = new Thread(&PromptTheUserAndDoTransition);
	gThreadP->Run(gThreadP);

	return 0;
}
static unsigned long Step(vector<RemoteProcedureCall::Parameter *> *v, void *user_dataP) {
	RemoteProcedureCall::Parameter *pReturn = (*v)[0];
	RemoteProcedureCall::Parameter *pMachine = (*v)[2];
	RemoteProcedureCall::Parameter *pSource = (*v)[3];
	RemoteProcedureCall::Parameter *pTransition = (*v)[4];
	RemoteProcedureCall::Parameter *pDestination = (*v)[5];

	cout << "Machine("<< pMachine->GetStringReference() << "):" << pSource->GetStringReference() << "-" << pTransition->GetStringReference() << "->" << pDestination->GetStringReference() << endl;
	cout << "Let's keep on playing" << endl;

	if (gThreadP && gThreadP->IsRunning()) 
		gThreadP->Stop();

	gThreadP = new Thread(&PromptTheUserAndDoTransition);
	gThreadP->Run(gThreadP);

	return 0;
}
static unsigned long Win(vector<RemoteProcedureCall::Parameter *> *v, void *user_dataP) {
	RemoteProcedureCall::Parameter *pReturn = (*v)[0];
	RemoteProcedureCall::Parameter *pMachine = (*v)[2];
	RemoteProcedureCall::Parameter *pSource = (*v)[3];
	RemoteProcedureCall::Parameter *pTransition = (*v)[4];
	RemoteProcedureCall::Parameter *pDestination = (*v)[5];

	cout << "Machine("<< pMachine->GetStringReference() << "):" << pSource->GetStringReference() << "-" << pTransition->GetStringReference() << "->" << pDestination->GetStringReference() << endl;
	cout << "You Won!!" << endl;
	cout << "Play a new game (Y/N)? : ";
	char  c;
	cin >> c;
	if (c == 'Y')
		gRpcMachineServerP->RpcCall("do_transition", RemoteProcedureCall::UINT64, gMachineId, RemoteProcedureCall::STRING, sStart, RemoteProcedureCall::END_OF_CALL);
	else exit(0);

	return 0;
}
static unsigned long GameOver(vector<RemoteProcedureCall::Parameter *> *v, void *user_dataP) {
	RemoteProcedureCall::Parameter *pReturn = (*v)[0];
	RemoteProcedureCall::Parameter *pMachine = (*v)[2];
	RemoteProcedureCall::Parameter *pSource = (*v)[3];
	RemoteProcedureCall::Parameter *pTransition = (*v)[4];
	RemoteProcedureCall::Parameter *pDestination = (*v)[5];

	cout << "Machine("<< pMachine->GetStringReference() << "):" << pSource->GetStringReference() << "-" << pTransition->GetStringReference() << "->" << pDestination->GetStringReference() << endl;
	cout << "You lost!!" << endl;
	cout << "Play a new game (Y/N)? : ";
	char  c;
	cin >> c;
	if (c == 'Y')
		gRpcMachineServerP->RpcCall("do_transition", RemoteProcedureCall::UINT64, gMachineId, RemoteProcedureCall::STRING, sStart, RemoteProcedureCall::END_OF_CALL);
	else exit(0);

	return 0;
}

int main(int argc, char **argv) {
	if (argc != 4) {
		cout << "usage:" << endl;
		cout << argv[0] << "\t client_address state_machine_address machine_json_definition_file" << endl;
		return -1;
	}

	cout << "argc :" << argc << endl;
	for (int i = 0; i < argc; i++)
		cout << "arg #" << i << ": " << argv[i] << endl;

	string client_addr = argv[1];
	string state_machine_addr = argv[2];
	string json_definition_filename = argv[3];

	ifstream file(json_definition_filename);
	if (!file.is_open()) {
		cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: couldn't open " << json_definition_filename << "!" << endl;
		return -1;
	}
	stringstream jsonStream;
	jsonStream << file.rdbuf();	

	RPCServer server(Transport::TCP, client_addr);
	gRpcMachineClientP = &server;
	
	server.RegisterProcedure("to_game_start", &GameStart);
	server.RegisterProcedure("to_one", &Step);
	server.RegisterProcedure("to_two", &Step);
	server.RegisterProcedure("to_three", &Step);
	server.RegisterProcedure("to_win", &Win);
	server.RegisterProcedure("to_game_over", &GameOver);

	RPCClient machineServer(Transport::TCP, state_machine_addr);
	gRpcMachineServerP = &machineServer;
	
	// tell the server to build this machine
	machineServer.RpcCall("build_machine", 
				   RemoteProcedureCall::STRING, jsonStream.str(), 	
				   RemoteProcedureCall::PTR, RemoteProcedureCall::UINT64, (uint64_t*)&gMachineId,
				   RemoteProcedureCall::END_OF_CALL); 			
	
	// tell the state machine to connect here after 0 seconds (the definition file sets a timeout on the first state!)
	machineServer.RpcCall("delayed_connect", 
				   RemoteProcedureCall::UINT64, gMachineId,
				   RemoteProcedureCall::STRING, client_addr, 			
				   RemoteProcedureCall::UINT16, 0,
				   RemoteProcedureCall::END_OF_CALL); 			

	server.IterateAndWait();

	return 0;
}




