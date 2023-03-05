#include <iostream>
#include <string>
#include <vector>

#include <RemoteProcedureCall.h>
#include <RPCServer.h>
#include <RPCClient.h>

#include <StateMachine.h>

using namespace std;

RPCServer 		*gRpcServerP = NULL;

// structure to connect to the state machine referenced by delayed_connect
typedef struct {
	StateMachine*	machineP;	// the machine asked to connect to
	string  		addr;		// this server addr
	uint16_t  		delay;		// after this delay
} DelayedConnectionInfo;

static void DelayedConnectionThread(void *paramP) {
	DelayedConnectionInfo*	infoP = (DelayedConnectionInfo*)paramP;

	if (infoP) {
		sleep(infoP->delay);

		// the client (to the state changes)
		if (infoP->machineP && !infoP->addr.empty())
			infoP->machineP->SetRpcClient(new RPCClient(Transport::TCP, infoP->addr));

		delete infoP;
	}
}

static unsigned long DelayedConnect(vector<RemoteProcedureCall::Parameter *> *v, void *user_dataP) {
	RemoteProcedureCall::Parameter *pReturn = (*v)[0];
	RemoteProcedureCall::Parameter *p1 = (*v)[1];
	RemoteProcedureCall::Parameter *p2 = (*v)[2];
	RemoteProcedureCall::Parameter *p3 = (*v)[3];
	if (!p1 || !p2 || !p3)
		return -1;

	static Thread thread(DelayedConnectionThread);
	thread.Run((void *)new DelayedConnectionInfo{(StateMachine*)p1->GetUInt64Reference(), p2->GetStringReference(), p3->GetUInt16Reference()});

	return 0;
}

static unsigned long DoTransition(vector<RemoteProcedureCall::Parameter *> *v, void *user_dataP) {
	RemoteProcedureCall::Parameter *pReturn = (*v)[0];
	RemoteProcedureCall::Parameter *p1 = (*v)[1];
	RemoteProcedureCall::Parameter *p2 = (*v)[2];
	if (!p1 || !p2)
		return -1;

	StateMachine* machineP = (StateMachine*)p1->GetUInt64Reference();
	if (machineP) 
		machineP->DoTransition(p2->GetStringReference());
	
	return 0;
}

static unsigned long BuildMachine(vector<RemoteProcedureCall::Parameter *> *v, void *user_dataP) {
	RemoteProcedureCall::Parameter *pReturn = (*v)[0];
	RemoteProcedureCall::Parameter *p1 = (*v)[1];
	RemoteProcedureCall::Parameter *p2 = (*v)[2];
	if (!p1 || !p2)
		return -1;

	string&	jsonString = p1->GetStringReference();
	stringstream jsonStream;
	jsonStream.write(jsonString.c_str(), jsonString.length());
	
	uint64_t& result = p2->GetUInt64Reference();
	result = (uint64_t)StateMachine::CreateFromDefinition(jsonStream);

	return 0;
}

int main(int argc, char **argv) {
	if (argc != 2) {
		cout << "usage:" << endl;
		cout << argv[0] << "\t server_address" << endl;
		return -1;
	}

	cout << "argc :" << argc << endl;
	for (int i = 0; i < argc; i++)
		cout << "arg #" << i << ": " << argv[i] << endl;

	string state_machine_addr = argv[1];

	// the state machine server
	RPCServer server(Transport::TCP, state_machine_addr);

	// start the StateMachine Server
	server.RegisterProcedure("do_transition", &DoTransition);
	server.RegisterProcedure("delayed_connect", &DelayedConnect);
	server.RegisterProcedure("build_machine", &BuildMachine);

	server.IterateAndWait();

	return 0;
}




