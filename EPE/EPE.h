#ifndef _EPE_H
#define _EPE_H

#include <string>
#include <map>

#include <Transport.h>
#include <RPCServer.h>
#include <RPCClient.h>

using namespace std;

#define EPE_TRACES 1
#define EPE_MODULE 6

/**
 * \class ezProtoEngine
 * \brief Provides EClients with an rpc service to manipulate protocol
 *        related objects (State Machines, Codecs, Messages, etc)
 */
class ezProtoEngine {
	RPCServer 						m_rpc_server; 	// the EClient calls come through this
	static map<string, RPCClient *> m_clients;		// the clients' rpcs

	static unsigned long RegisterEClient(vector<RemoteProcedureCall::Parameter *> *v);
	static unsigned long UnregisterEClient(vector<RemoteProcedureCall::Parameter *> *v);
	static unsigned long CreateStateMachine(vector<RemoteProcedureCall::Parameter *> *v);
	static unsigned long DestroyStateMachine(vector<RemoteProcedureCall::Parameter *> *v);
	static unsigned long DoTransition(vector<RemoteProcedureCall::Parameter *> *v);

public:
	ezProtoEngine(Transport::TransportType type, string &server_address) : m_rpc_server(type, server_address) {
		m_rpc_server.RegisterProcedure("RegisterEClient", (RemoteProcedure *)&RegisterEClient);
		m_rpc_server.RegisterProcedure("UnregisterEClient", (RemoteProcedure *)&UnregisterEClient);
		m_rpc_server.RegisterProcedure("CreateStateMachine", (RemoteProcedure *)&CreateStateMachine);
		m_rpc_server.RegisterProcedure("DestroyStateMachine", (RemoteProcedure *)&DestroyStateMachine);
		m_rpc_server.RegisterProcedure("DoTransition", (RemoteProcedure *)&DoTransition);
	}

	void Run() {
		m_rpc_server.IterateAndWait();
	}

	void Stop() {
		for (map<string, RPCClient *>::iterator i = m_clients.begin(); i != m_clients.end(); i++)
			delete (*i).second;
		m_clients.clear();
	}
};

#endif /* _EPE_H */
