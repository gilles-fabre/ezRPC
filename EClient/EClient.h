#ifndef _ECLIENT_H
#define _ECLIENT_H

#include <string>

#include <Transport.h>
#include <Thread.h>
#include <RPCServer.h>
#include <RPCClient.h>

using namespace std;

#define ECLIENT_TRACES 1
#define ECLIENT_MODULE 7

typedef unsigned long SMID;

/**
 * \class EClient
 * \brief EClients instances can manipulate protocol related objects (State Machines,
 * 		  Codecs, Messages, etc) seamlessly through a bidirectionnal rpc mecanism.
 */
class EClient {
	string	  				 m_client_address;
	Transport::TransportType m_transport_type;
	RPCServer 				 m_callbacks_server; 	// callbacks from the EPE will go through this
	RPCClient 				 m_epe_client;			// calls to the EPE will go through this
	Thread					 m_callbacks_server_thread;

	// SERVER THREAD CALLBACK
	static void CallbacksServerCallback(void *paramP) {
		RPCServer *serverP = (RPCServer *)paramP;
		if (serverP)
			serverP->RunAndWait(); // no need to iterate here, we have a single client
	}

	// STATE MACHINES CALLBACK

	static unsigned long TransitionCallback(vector<RemoteProcedureCall::Parameter *> *v);

	// CODEC CALLBACKS

	static unsigned long GetMessageFieldCallback(vector<RemoteProcedureCall::Parameter *> *v);
	static unsigned long MessageReceivedCallback(vector<RemoteProcedureCall::Parameter *> *v);

	// DATA EXCHANGE (with peer equipement) CALLBACK

	static unsigned long SendDataCallback(vector<RemoteProcedureCall::Parameter *> *v);
	static unsigned long ReceiveDataCallback(vector<RemoteProcedureCall::Parameter *> *v);

public:
	EClient(Transport::TransportType transport_type, string &server_address, string &client_address) :
		m_epe_client(transport_type, server_address),
		m_callbacks_server(transport_type, client_address),
		m_callbacks_server_thread(&CallbacksServerCallback) {
		m_client_address = client_address;
		m_transport_type = transport_type;
	}

	virtual ~EClient() {
		m_callbacks_server.Stop();
		m_epe_client.Stop();
	}

	virtual void Run() {
		// run the EClient's rpc server
		m_callbacks_server_thread.Run(&m_callbacks_server);

		// registers the EClient's callbacks
		m_callbacks_server.RegisterProcedure("TransitionCallback", (RemoteProcedure *)&TransitionCallback);
		m_callbacks_server.RegisterProcedure("GetMessageFieldCallback", (RemoteProcedure *)&GetMessageFieldCallback);
		m_callbacks_server.RegisterProcedure("MessageReceivedCallback", (RemoteProcedure *)&MessageReceivedCallback);
		m_callbacks_server.RegisterProcedure("SendDataCallback", (RemoteProcedure *)&SendDataCallback);
		m_callbacks_server.RegisterProcedure("ReceiveDataCallback", (RemoteProcedure *)&ReceiveDataCallback);

		// register this EClient against the EPE
		m_epe_client.RpcCall("RegisterEClient",
							 RemoteProcedureCall::BYTE,
							 (unsigned char)m_transport_type,
							 RemoteProcedureCall::STRING,
							 &m_client_address,
							 RemoteProcedureCall::END_OF_CALL);
	}

	virtual void Stop() {
		// unregister this EClient
		m_epe_client.RpcCall("UnregisterEClient",
							 RemoteProcedureCall::STRING,
							 &m_client_address,
							 RemoteProcedureCall::END_OF_CALL);

		m_epe_client.Stop();
		m_callbacks_server.Stop();
	}

	// implement EClient virtual APIs (to SMs, MCs)
	unsigned long CreateStateMachine(const string &json_definition_file, void *user_dataP = NULL);
	unsigned long DestroyStateMachine(SMID state_machine_id);
	unsigned long DoTransition(SMID state_machine_id, const string &transition_name);
};


#endif /* _ECLIENT_H */
