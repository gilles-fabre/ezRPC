#ifndef _RPC_CLIENT_H
#define _RPC_CLIENT_H

#include <string>
#include <cstdarg>

#include <log2reporter.h>
#include <Transport.h>

#include "RemoteProcedureCall.h"

using namespace std;

#define RPCCLIENT_TRACES 1
#define RPCCLIENT_MODULE 0x5

#ifdef WIN32
#pragma warning(disable:4251)
#ifdef _EXPORTING
#define DECLSPEC __declspec(dllexport)
#else
#define DECLSPEC __declspec(dllimport)
#endif
#endif

typedef void AsyncReplyProcedure(unsigned long asyncId, unsigned long result);

/**
 * \class RPCClient
 * \brief Provides the user with a high abstraction level Remote Procedure Calls service.
 */
#ifdef WIN32
class	DECLSPEC RPCClient {
#else
class	RPCClient {
#endif
	Transport 		 														*m_transportP; 				// transport used to talk to the peer server
	RemoteProcedureCall												*m_rpcP;							// rpc underlying abstraction

public:
	/**
	 * \fn RPCClient(Transport::TransportType transport_type,  string &server_address)
	 * \brief RPCClient constructor
	 * 
	 * \param transport_type is one of the Transport::TransportType enum values
	 * \param server_address is the server address (for the given transport_type)
	*/
	RPCClient(Transport::TransportType transport_type,  const string& server_address) {
		m_transportP = Transport::CreateTransport(transport_type);
		Link *linkP = m_transportP ? m_transportP->LinkRequest((const string&)server_address) : NULL;
		m_rpcP = linkP ? new RemoteProcedureCall(linkP) : NULL;
	}

	/**
	 * \fn virtual ~RPCClient()
	 * \brief RPCClient destructor. Closes and destroys the associated RPC 
	 * 		  and Transport members.
	*/
	virtual ~RPCClient() {
#ifdef RPCCLIENT_TRACES
		LogText(RPCCLIENT_MODULE, 0, true, "RPCClient::~RpcClient()");
#endif
		if (m_transportP) {
#ifdef RPCCLIENT_TRACES
			LogText(RPCCLIENT_MODULE, 4, true, "Closing transport");
#endif
			m_transportP->Close();
			delete m_transportP;
		}
		m_transportP = NULL;

		if (m_rpcP)
			delete m_rpcP;
		m_rpcP = NULL;
	}

	/**
	 * \fn void Stop()
	 * \brief Stops the associated transport.
	*/
	void Stop() {
		if (m_transportP)
			m_transportP->Close();
	}

	class AsyncReplyHandlerParameters {
	public:
		RemoteProcedureCall*		m_rpcP;							// rpc underlying abstraction
		AsyncReplyProcedure*		m_procedureP;				// async call completion callback
		unsigned long						m_asyncId;					// processed async call identifier
		std::shared_ptr<Thread> m_thread;						// processing thread (to ref the shared ptr)
		string									m_function;					// called function
		std::va_list					  m_args;							// args to the called function

		AsyncReplyHandlerParameters(RemoteProcedureCall* rpc, AsyncReplyProcedure* procedureP, unsigned long asyncId, std::shared_ptr<Thread> thread, string func_name, std::va_list vl) {
			m_rpcP = rpc;
			m_procedureP = procedureP;
			m_asyncId = asyncId;
			m_thread = thread;
			m_function = func_name;
			m_args = vl;
		}
	};

	static void AsyncRpcCallHandler(void* paramP);

	unsigned long RpcCall(string func_name, ...);
	unsigned long RpcCallAsync(AsyncReplyProcedure* procedureP, string func_name, ...);
};

#endif /* _RPC_CLIENT_H */
