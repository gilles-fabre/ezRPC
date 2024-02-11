#ifndef _RPC_SERVER_H
#define _RPC_SERVER_H

#include <vector>
#include <string>
#include <map>
#include <log2reporter.h>

#include <Thread.h>
#include <Transport.h>

#include "RemoteProcedureCall.h"

using namespace std;

#define RPCSERVER_TRACES 1
#define RPCSERVER_MODULE 0x6

#ifdef WIN32
#pragma warning(disable:4251)
#ifdef _EXPORTING
#define DECLSPEC __declspec(dllexport)
#else
#define DECLSPEC __declspec(dllimport)
#endif
#endif

/**
 * \class RPCServer
 * \brief An instance of RPCServer provides RPCClients with an address[:port] to connect
 *        to, over a given transport, and obtain a dedicated link (one per RPCClient).
 *        The latter can be used by the RPCClient to remotely invoke procedures of the
 *        (linked) RPCServer. Procedures can be dynamically added or removed from an
 *        RPCServer instance. See RemoteProcedureCall to get the details of the remote
 *        procedure invocation.
 *
 *
 */
#ifdef WIN32
class	DECLSPEC RPCServer {
#else
class	RPCServer {
#endif
	Transport*						m_transportP;			// transport used to talk to this server
	string			 				m_address;				// associated address
	void*							m_user_dataP;			// optional user data passed to rpc procedures
	Thread*							m_listening_threadP;	// responsible for listening and spawning service threads
	vector<Thread*> 				m_serving_threads;		// all running and 'linked' service threads, waiting for remote procedure calls
	map<string, RemoteProcedure*> 	m_rpc_map;				// the procedures to call based on their names

	/**
		 * \class ServiceParameters
		 * \brief An instance of this is passed to each new ServiceThread in order to
		 *        give it the necessary information to serve the linked client.
		 */
	class ServiceParameters {
	public:
		RPCServer*	m_serverP;
		Link*		m_linkP;

		/**
		 * \fn ServiceParameters(RPCServer *serverP, Link *linkP)
		 * \brief constructs a service parameters object giving
		 *        access to the 'parent' RPCServer and the link
		 *        to the client.
		 *
		 * \param serverP points to the RPC Server
		 * \param linkP points the tranport link
		 */
		ServiceParameters(RPCServer* serverP, Link* linkP) {
			m_serverP = serverP;
			m_linkP = linkP;
		}
	};

	static void ListeningCallback(void *);					// threads' functions
	static void CallServiceAndReply(RemoteProcedureCall& rpc, RemoteProcedure* procP, AsyncID asyncId, const string& func_name, shared_ptr<ServiceParameters> params, shared_ptr<vector<RemoteProcedureCall::Parameter*>> rpc_params);
	static void ServiceCallback(void *);	

public:
	/**
	 * \fn RPCServer(Transport::TransportType transport_type,  string &server_address, void *user_dataP = NULL)
	 * \brief RPCServer constructor
	 * 
	 * \param transport_type is one of the Transport::TransportType enum values
	 * \param server_address is the server address (for the given transport_type)
	 * \param user_dataP is a pointer to user data passed in every subsequent RPC call. Set to NULL by default
	*/
	RPCServer(Transport::TransportType transport_type, const string &address, void *user_dataP = NULL) {
		m_listening_threadP = NULL;
		m_transportP = Transport::CreateTransport(transport_type);
		m_address = address;
		m_user_dataP = user_dataP;
	}

	/**
	 * \fn virtual ~RPCServer()
	 * \brief RPCClient destructor. Closes and destroys the associated RPC 
	 * 		  and Transport members.
	*/
	virtual ~RPCServer() {
#ifdef RPCSERVER_TRACES
		LogText(RPCSERVER_MODULE, 0, true, "RPCServer::~RpcServer()");
#endif
		Stop();

		if (m_transportP) 
			delete m_transportP;

		m_transportP = NULL;
	}

	/**
	 * \fn void RegisterProcedure(string name, RemoteProcedure *procedureP)
	 * \brief Registers a procedure so it can be called by a Remote Client.
	 * 
	 * \param name is the name of the registered procedure
	 * \param procedureP is a pointer to the registered procedure callback
	*/
	void RegisterProcedure(string name, RemoteProcedure *procedureP) {
		m_rpc_map.insert(std::pair<string, RemoteProcedure *>(name, procedureP));
	}

	/**
	 * \fn void UnregisterProcedure(string name)
	 * \brief Unregisters a procedure.
	 * 
	 * \param name is the name of the unregistered procedure
	*/
	void UnregisterProcedure(string name) {
		m_rpc_map.erase(name);
	}

	/**
	 * \fn void Run()
	 * \brief One shot run of the server, will accept a single connection
	 *        and then serve the connected client. Exits when the connection
	 * 		  drops.
	*/
	void Run() {
		if (!m_listening_threadP)
			m_listening_threadP = new Thread(&ListeningCallback);

		if (!m_listening_threadP->IsRunning() && !m_listening_threadP->IsAskedToStop())
			m_listening_threadP->Run((void *)this);
	}

	/**
	 * \fn void RunAndWait()
	 * \brief One shot run of the server, will accept a single connection
	 *        and then serve the connected client. Exits and Wait when the connection
	 * 		  drops.
	*/
	void RunAndWait() {
		if (!m_listening_threadP)
			m_listening_threadP = new Thread(&ListeningCallback);

		if (!m_listening_threadP->IsRunning() && !m_listening_threadP->IsAskedToStop())
			m_listening_threadP->RunAndWait((void *)this);
	}

	/**
	 * \fn void Iterate()
	 * \brief Iterates on Run.
	*/
	void Iterate() {
		if (!m_listening_threadP)
			m_listening_threadP = new Thread(&ListeningCallback);

		if (!m_listening_threadP->IsRunning() && !m_listening_threadP->IsAskedToStop())
			m_listening_threadP->Iterate((void *)this);
	}

	/**
	 * \fn void Iterate()
	 * \brief Iterates on Run then Wait.
	*/
	void IterateAndWait() {
		if (!m_listening_threadP)
			m_listening_threadP = new Thread(&ListeningCallback);

		if (!m_listening_threadP->IsRunning() && !m_listening_threadP->IsAskedToStop())
			m_listening_threadP->IterateAndWait((void *)this);
	}

	/**
	 * \fn void Stop()
	 * \brief Stops the associated transport.
	*/
	void Stop() {
#ifdef RPCSERVER_TRACES
		LogText(RPCSERVER_MODULE, 0, true, "RPCServer::Stop()");
#endif

		if (m_transportP) {
#ifdef RPCSERVER_TRACES
			LogText(RPCSERVER_MODULE, 4, true, "Closing transport");
#endif
			m_transportP->Close();
		}

		if (m_listening_threadP) {
			if (m_listening_threadP->IsRunning())
				m_listening_threadP->Stop();

			delete m_listening_threadP;
			m_listening_threadP = NULL;
		}

		for (vector<Thread *>::iterator i = m_serving_threads.begin(); i != m_serving_threads.end(); i++)
			delete (*i);
		m_serving_threads.clear();
	}
};

#endif /* _RPC_SERVER_H */
