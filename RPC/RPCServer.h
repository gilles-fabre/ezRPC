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

/**
 * \class RPCServer
 * \brief An instance of RPCServer provides RPCClients with an address[:port] to connect
 *        to, over a given transport, and obtain a dedicated link (one per RPCClient).
 *        The latter can be used by the RPCClient to remotely invoke procedures of the
 *        (linked) RPCServer. Procedures can be dynamically added or removed from an
 *        RPCServer instance. See RemoteProcedureCall to get the details of the remote
 *        procedurlibjson lgples invocation.
 *
 *
 */
class RPCServer {
	Transport 		 				*m_transportP;			// transport used to talk to this server
	string			 				m_address;				// associated address
	void							*m_user_dataP;			// optional user data passed to rpc procedures
	Thread 			 				*m_listening_threadP;	// responsible for listening and spawning service threads
	vector<Thread *> 				m_serving_threads;		// all running and 'linked' service threads, waiting for remote procedure calls
	map<string, RemoteProcedure *> 	m_rpc_map;				// the procedures to call based on their names

	static void ListeningCallback(void *);					// threads' functions
	static void ServiceCallback(void *);

	/**
	 * \class ServiceParameters
	 * \brief An instance of this is passed to each new ServiceThread in order to
	 *        give it the necessary information to serve the linked client.
	 */
	class ServiceParameters {
	public:
		RPCServer *m_serverP;
		Link 	  *m_linkP;

		/**
		 * \fn ServiceParameters(RPCServer *serverP, Link *linkP)
		 * \brief constructs a service parameters object giving
		 *        access to the 'parent' RPCServer and the link
		 *        to the client.
		 */
		ServiceParameters(RPCServer *serverP, Link *linkP) {
			m_serverP = serverP;
			m_linkP = linkP;
		}
	};

public:
	RPCServer(Transport::TransportType transport_type,  string &address, void *user_dataP = NULL) {
		m_listening_threadP = NULL;
		m_transportP = Transport::CreateTransport(transport_type);
		m_address = address;
		m_user_dataP = user_dataP;
	}

	virtual ~RPCServer() {
#ifdef RPCSERVER_TRACES
		LogText(RPCSERVER_MODULE, 0, true, "RPCServer::~RpcServer()");
#endif
		Stop();

		if (m_transportP) {
#ifdef RPCSERVER_TRACES
			LogText(RPCSERVER_MODULE, 4, true, "Closing transport");
#endif
			m_transportP->Close();
			delete m_transportP;
		}
		m_transportP = NULL;
	}

	void RegisterProcedure(string name, RemoteProcedure *procedureP) {
		m_rpc_map.insert(std::pair<string, RemoteProcedure *>(name, procedureP));
	}

	void UnregisterProcedure(string name) {
		m_rpc_map.erase(name);
	}

	void Run() {
		if (!m_listening_threadP)
			m_listening_threadP = new Thread(&ListeningCallback);

		if (!m_listening_threadP->IsRunning() && !m_listening_threadP->IsAskedToStop())
			m_listening_threadP->Run((void *)this);
	}

	void RunAndWait() {
		if (!m_listening_threadP)
			m_listening_threadP = new Thread(&ListeningCallback);

		if (!m_listening_threadP->IsRunning() && !m_listening_threadP->IsAskedToStop())
			m_listening_threadP->RunAndWait((void *)this);
	}

	void Iterate() {
		if (!m_listening_threadP)
			m_listening_threadP = new Thread(&ListeningCallback);

		if (!m_listening_threadP->IsRunning() && !m_listening_threadP->IsAskedToStop())
			m_listening_threadP->Iterate((void *)this);
	}

	void IterateAndWait() {
		if (!m_listening_threadP)
			m_listening_threadP = new Thread(&ListeningCallback);

		if (!m_listening_threadP->IsRunning() && !m_listening_threadP->IsAskedToStop())
			m_listening_threadP->IterateAndWait((void *)this);
	}

	void Stop() {
#ifdef RPCSERVER_TRACES
		LogText(RPCSERVER_MODULE, 0, true, "RPCServer::Stop()");
#endif
		if (m_listening_threadP) {
			if (m_listening_threadP->IsRunning())
				m_listening_threadP->StopAndWait();

			delete m_listening_threadP;
			m_listening_threadP = NULL;
		}

		for (vector<Thread *>::iterator i = m_serving_threads.begin(); i != m_serving_threads.end(); i++)
			delete (*i);
		m_serving_threads.clear();
	}
};

#endif /* _RPC_SERVER_H */
