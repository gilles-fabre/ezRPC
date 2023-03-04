#include <iostream>
#include <string>
#include <vector>

#include <RemoteProcedureCall.h>
#include <RPCServer.h>
#include <RPCClient.h>

#include <StateMachine.h>

using namespace std;

StateMachine 	*gStateMachineP = NULL;
RPCServer 		*gRpcServerP = NULL;

static void DelayedConnectionThread(void *paramP) {
	std::string* addrP = (std::string*)paramP; 

	sleep(1);

	// the client (to the state changes)
	gStateMachineP->SetRpcClient(new RPCClient(Transport::TCP, *addrP));

	delete addrP;
}

static unsigned long DelayedConnect(vector<RemoteProcedureCall::Parameter *> *v, void *user_dataP) {
	RemoteProcedureCall::Parameter *pReturn = (*v)[0];
	RemoteProcedureCall::Parameter *p1 = (*v)[1];

	static Thread thread(DelayedConnectionThread);
	thread.Run((void *)new std::string(p1->GetStringValue()));

	return 0;
}

static unsigned long DoTransition(vector<RemoteProcedureCall::Parameter *> *v, void *user_dataP) {
	RemoteProcedureCall::Parameter *pReturn = (*v)[0];
	RemoteProcedureCall::Parameter *p1 = (*v)[1];

	gStateMachineP->DoTransition(p1->GetStringValue());
	
	return 0;
}

int main(int argc, char **argv) {
	if (argc != 3) {
		cout << "usage:" << endl;
		cout << argv[0] << "\t server_address json_definition_file" << endl;
		return -1;
	}

	cout << "argc :" << argc << endl;
	for (int i = 0; i < argc; i++)
		cout << "arg #" << i << ": " << argv[i] << endl;

	string state_machine_addr = argv[1];
	string json_file = argv[2];

	gStateMachineP = StateMachine::CreateFromDefinition(json_file);
	if (!gStateMachineP)
		exit(-1);

	// the state machine server
	RPCServer server(Transport::TCP, state_machine_addr);

	// start the StateMachine Server
	server.RegisterProcedure("do_transition", &DoTransition);
	server.RegisterProcedure("delayed_connect", &DelayedConnect);

	server.IterateAndWait();

	return 0;
}




