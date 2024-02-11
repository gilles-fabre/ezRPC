#include <iostream>
#include <string>
#include <vector>

#include <RemoteProcedureCall.h>
#include <RPCServer.h>
#include <RPCClient.h>

#include <StateMachine.h>

using namespace std;

RPCServer* gRpcServerP = NULL;

// structure to connect to the state machine referenced by delayed_connect
typedef struct {
	StateMachine*	machineP;	// the machine asked to connect to
	string  		addr;		// this server addr
	uint16_t  		delay;		// after this delay
} DelayedConnectionInfo;

/**
 * \fn void DelayedConnectionThread(void* paramP) 
 * \brief Delayed connection thread, invoked by the client to request the given
 * 		  state machine to connect to itself, so the state machine can invoke
 * 		  the client's state transition callbacks.
 *
 * \param paramP is a pointer to a DelayedConnectionInfo structure, containing the
 * 				 target state machine, the client IP_V4:port address, and the 
 * 				 connection delay in seconds. 
 */
static void DelayedConnectionThread(void* paramP) {
	DelayedConnectionInfo*	infoP = (DelayedConnectionInfo*)paramP;

	if (infoP) {
		sleep(infoP->delay);

		// the client (to the state changes)
		if (infoP->machineP && !infoP->addr.empty())
			infoP->machineP->SetRpcClient(new RPCClient(Transport::TCP, infoP->addr));

		delete infoP;
	}
}

/**
 * \fn unsigned long DelayedConnect(vector<RemoteProcedureCall::Parameter*>* v, void* user_dataP) 
 * \brief Delayed connection RPC service, invoked by the client to request the given
 * 		  state machine to connect to itself, so the state machine can invoke
 * 		  the client's state transition callbacks.
 * 
 * \param (*v)[0] RemoteProcedureCall::UIN64 + target StateMachine ID
 * \param (*v)[1] RemoteProcedureCall::STRING + client IPV4:port address
 * \param (*v)[2] RemoteProcedureCall::UINT16 + connection delay in seconds
 * 
 * \return 0 is everything went fine, -1 else.
*/
static unsigned long DelayedConnect(vector<RemoteProcedureCall::Parameter*>* v, void* user_dataP) {
	RemoteProcedureCall::Parameter* pReturn = (*v)[0];
	RemoteProcedureCall::Parameter* p1 = (*v)[1];
	RemoteProcedureCall::Parameter* p2 = (*v)[2];
	RemoteProcedureCall::Parameter* p3 = (*v)[3];
	if (!p1 || !p2 || !p3)
		return -1;

	static Thread thread(DelayedConnectionThread);
	thread.Run((void *)new DelayedConnectionInfo{(StateMachine*)p1->GetReference<uint64_t>(), p2->GetReference<string>(), p3->GetReference<uint16_t>()});

	return 0;
}

/**
 * \fn unsigned long ReleaseMachine(vector<RemoteProcedureCall::Parameter*>* v, void* user_dataP)
 * \brief Release the given machine so the garbage collector can trash it.
 * 
 * \param (*v)[0] RemoteProcedureCall::UIN64 + target StateMachine ID
 * 
 * \return 0 is everything went fine, -1 else.
*/
static unsigned long ReleaseMachine(vector<RemoteProcedureCall::Parameter*>* v, void* user_dataP) {
	RemoteProcedureCall::Parameter* pReturn = (*v)[0];
	RemoteProcedureCall::Parameter* p1 = (*v)[1];
	RemoteProcedureCall::Parameter* p2 = (*v)[2];
	if (!p1 || !p2)
		return -1;

	StateMachine* machineP = (StateMachine*)p1->GetReference<uint64_t>();
	if (machineP) 
		machineP->Release();
	
	return 0;
}

/**
 * \fn unsigned long DoTransition(vector<RemoteProcedureCall::Parameter*>* v, void* user_dataP)
 * \brief Instruct the given machine to do a transition.
 * 
 * \param (*v)[0] RemoteProcedureCall::UIN64 + target StateMachine ID
 * \param (*v)[1] RemoteProcedureCall::STRING + transition name
 * 
 * \return 0 is everything went fine, -1 else.
*/
static unsigned long DoTransition(vector<RemoteProcedureCall::Parameter*>* v, void* user_dataP) {
	RemoteProcedureCall::Parameter* pReturn = (*v)[0];
	RemoteProcedureCall::Parameter* p1 = (*v)[1];
	RemoteProcedureCall::Parameter* p2 = (*v)[2];
	if (!p1 || !p2)
		return -1;

	StateMachine* machineP = (StateMachine*)p1->GetReference<uint64_t>();
	if (machineP) 
		machineP->DoTransition(p2->GetReference<string>());
	
	return 0;
}

/**
 * \fn unsigned long BuildMachine(vector<RemoteProcedureCall::Parameter*>* v, void* user_dataP)
 * \brief Asks the server to build a State Machine based on the passed json definition.
 * 
 * \param (*v)[0] RemoteProcedureCall::PTR + RemoteProcedureCall::UIN64 + ptr to returned StateMachine ID. 
 * 				  not set if anything goes south (check the return value).
 * \param (*v)[1] RemoteProcedureCall::STRING + json definition
 * 
 * \return 0 is everything went fine, -1 else.
*/
static unsigned long BuildMachine(vector<RemoteProcedureCall::Parameter*>* v, void* user_dataP) {
	RemoteProcedureCall::Parameter* pReturn = (*v)[0];
	RemoteProcedureCall::Parameter* p1 = (*v)[1];
	RemoteProcedureCall::Parameter* p2 = (*v)[2];
	if (!p1 || !p2)
		return -1;

	string&	jsonString = p1->GetReference<string>();
	stringstream jsonStream;
	jsonStream.write(jsonString.c_str(), jsonString.length());
	
	uint64_t& result = p2->GetReference<uint64_t>();
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
	server.RegisterProcedure("release_machine", &ReleaseMachine);

	server.IterateAndWait();

	return 0;
}




