#include <signal.h>

#include <iostream>
#include <string>

#include <log2reporter.h>
#include <StateMachine.h>

#include "EPE.h"

ezProtoEngine *g_epeP = NULL;

map<string, RPCClient *> ezProtoEngine::m_clients;		// the clients' rpcs

/**
 * \fn unsigned long ezProtoEngine::RegisterEClient(vector<RemoteProcedureCall::Parameter *> *v)
 * \brief Registers an EClient's RPCServer against the EPE. This enables the EPE to callback into a
 *        registered EClient's RPCServer's procedures.
 *
 * \param v is a vector of Remote Procedure Call Parameters, which contains the following
 *        elements:
 *        	result return address : unsigned long (unused, do not modify)
 *        	client address : string, uniquely identifies the EClient address on the transport
 *			transport type : TCP or FILE
 *
 * \return 0 if everything went fine, else return -1
 */
unsigned long ezProtoEngine::RegisterEClient(vector<RemoteProcedureCall::Parameter *> *v) {
	// check params
	if (v->size() != 3)
		return (unsigned long)-1;

	RemoteProcedureCall::Parameter *transport_typeP = (*v)[1];  // transport type
	RemoteProcedureCall::Parameter *client_addressP = (*v)[2];	// client's rpc server address

#ifdef EPE_TRACES
	LogVText(EPE_MODULE, 0, true, "ezProtoEngine::RegisterEClient(%u, %s)", transport_typeP->GetByteValue(), client_addressP->GetStringValue().c_str());
#endif

	// check parameters
	string &client_address = client_addressP->GetStringValue();
	if (client_address.empty() || m_clients[client_address])
		return (unsigned long)-1;

	// client callbacks are implicit....
	// but we need to talk to the client rpcservers (for callbacks)
	RPCClient *rpcP = new RPCClient((Transport::TransportType)transport_typeP->GetByteValue(), client_address);
	if (rpcP)
		m_clients[client_address] = rpcP;

#ifdef EPE_TRACES
	LogVText(EPE_MODULE, 0, true, "registered client: %s", client_address.c_str());
#endif

	return 0;
}

/**
 * \fn unsigned long ezProtoEngine::UnregisterEClient(vector<RemoteProcedureCall::Parameter *> *v)
 * \brief Unregisters an EClient's RPCServer.
 *
 * \param v is a vector of Remote Procedure Call Parameters, which contains the following
 *        elements:
 *        	result return address : unsigned long (unused, do not modify)
 *        	client address : string, uniquely identifies the EClient address on the transport
 *
 * \return 0 if everything went fine, else return -1
 */
unsigned long ezProtoEngine::UnregisterEClient(vector<RemoteProcedureCall::Parameter *> *v) {
	// check params
	if (v->size() != 2)
		return (unsigned long)-1;

	RemoteProcedureCall::Parameter *client_addressP = (*v)[1];	// client's rpc server address

#ifdef EPE_TRACES
	LogVText(EPE_MODULE, 0, true, "ezProtoEngine::UnregisterEClient(%s)", client_addressP->GetStringValue().c_str());
#endif

	// check parameters
	string &client_address = client_addressP->GetStringValue();
	if (client_address.empty() || !m_clients[client_address])
		return (unsigned long)-1;

	delete m_clients[client_address];
	m_clients.erase(client_address);

#ifdef EPE_TRACES
	LogVText(EPE_MODULE, 0, true, "unregistered client: %s", client_address.c_str());
#endif

	return 0;
}

/**
 * \fn unsigned long ezProtoEngine::CreateStateMachine(vector<RemoteProcedureCall::Parameter *> *v)
 * \brief Creates a state machine based on the given json definition file
 *
 * \param v is a vector of Remote Procedure Call Parameters, which contains the following
 *        elements:
 *        	result return address : unsigned long (unused, do not modify)
 *  		client address : the client's rpcserver address
 *  		user data pointer : the user data pointer associated with the state machine (is passed
 *  						    to all transition callbacks
 *
 * \return the newly created state machine address or NULL if an error occurred
 */
unsigned long ezProtoEngine::CreateStateMachine(vector<RemoteProcedureCall::Parameter *> *v) {
	// check params
	if (v->size() != 4)
		return 0;

	RemoteProcedureCall::Parameter *client_addressP = (*v)[1];	// client's rpc server address
	RemoteProcedureCall::Parameter *json_filenameP = (*v)[2];	// json state machine definition filename
	RemoteProcedureCall::Parameter *user_data = (*v)[3];		// user data pointer to pass to the state machine

	string 		client_address = client_addressP->GetStringValue();
	string 		json_filename = json_filenameP->GetStringValue();
	void		*user_dataP = (void *)user_data->GetUInt64Value();

	if (client_address.empty() || json_filename.empty())
		return 0;

	// find client from address.
	RPCClient *rpcP = m_clients[client_address];
	if (!rpcP)
		return 0;

	StateMachine *smP = StateMachine::CreateFromDefinition(json_filename);
	if (!smP)
		return 0;

	if (user_dataP)
		smP->SetUserData(user_dataP);

	// so the SM can callback the client
	smP->SetRpcClient(rpcP);

	return (uint64_t)smP;
}

/**
 * \fn unsigned long ezProtoEngine::DestroyStateMachine(vector<RemoteProcedureCall::Parameter *> *v)
 * \brief Destroys the given state machine
 *
 * \param v is a vector of Remote Procedure Call Parameters, which contains the following
 *        elements:
 *        	result return address : unsigned long (unused, do not modify)
 *  		client address : the client's rpcserver address
 *  		state machine address : the address of the state machine to release
 *
 * \return 0 or (unsigned long)-1 if an error occurred
 */
unsigned long ezProtoEngine::DestroyStateMachine(vector<RemoteProcedureCall::Parameter *> *v) {
	// check params
	if (v->size() != 3)
		return (unsigned long)-1;

	RemoteProcedureCall::Parameter *client_addressP = (*v)[1];	// client's rpc server address
	RemoteProcedureCall::Parameter *smidP = (*v)[2];			// state machine id

	string 			client_address = client_addressP->GetStringValue();
	StateMachine 	*smP = (StateMachine *)smidP->GetUInt64Value();

	if (client_address.empty() || !smP)
		return (unsigned long)-1;

	// ask the SM to delete itself
	smP->Release();

	return 0;
}

/**
 * \fn unsigned long ezProtoEngine::DoTransition(vector<RemoteProcedureCall::Parameter *> *v)
 * \brief Commands the given state machine to do the required transition
 *
 * \param v is a vector of Remote Procedure Call Parameters, which contains the following
 *        elements:
 *        	result return address : unsigned long (unused, do not modify)
 *  		state machine address : the address of the state machine to release
 *  		transition name : the name of the required transition
 *
 * \return 0 or (unsigned long)-1 if an error occurred
 */
unsigned long ezProtoEngine::DoTransition(vector<RemoteProcedureCall::Parameter *> *v) {
	// check params
	if (v->size() != 4)
		return (unsigned long)-1;

	RemoteProcedureCall::Parameter *client_addressP = (*v)[1];	// client's rpc server address
	RemoteProcedureCall::Parameter *smidP = (*v)[2];			// state machine id
	RemoteProcedureCall::Parameter *transitionP = (*v)[3];		// transition name

	string 			client_address = client_addressP->GetStringValue();
	string 			transition = transitionP->GetStringValue();
	StateMachine 	*smP = (StateMachine *)smidP->GetUInt64Value();

	if (client_address.empty() || transition.empty() || !smP)
		return (unsigned long)-1;

	// so the SM can callback the client
	smP->DoTransition(transition);

	return 0;
}

/**
 * \fn void signal_callback_handler(int signum)
 * \brief handles program termination by signal
 *
 * \param signum is the raised signal id
 */
void signal_callback_handler(int signum) {
	   cout << "Caught signal " << signum << ", now cleaning up and exiting..." << endl;

	   // cleanup
	   if (g_epeP)
		   g_epeP->Stop();

	   // terminate program
	   exit(signum);
}

int main(int argc, char **argv) {
   // register signal and signal handler
   signal(SIGINT, signal_callback_handler);

	if (argc < 3) {
		cout << "usage:" << endl;
		cout << "\tEPE <tcp|file> server_address" << endl;
		return -1;
	}

	Transport::TransportType 	type;
	string 						type_string = argv[1];
	string 						server_address = argv[2];

	if (type_string == "tcp")
		type = Transport::TCP;
	else if (type_string == "file")
		type = Transport::FILE;
	else {
		cout << "usage:" << endl;
		cout << "\tEPE <tcp|file> server_address" << endl;
		return -1;
	}

	// #### add exceptions
	ezProtoEngine epe(type, server_address);
	g_epeP = &epe;
	epe.Run();

	return 0;
}
