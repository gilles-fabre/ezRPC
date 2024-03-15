#ifndef _RPC_CLIENT_H
#define _RPC_CLIENT_H

#include <string>
#include <cstdarg>
#include <memory>
#include <vector>
#include <mutex>
#include <unordered_map>

#include <log2reporter.h>
#include <Transport.h>

#include "RemoteProcedureCall.h"

using namespace std;

#ifdef _DEBUG
#define RPCCLIENT_TRACES 1
#endif

#define RPCCLIENT_MODULE 0x5

#ifdef WIN32
#pragma warning(disable:4251)
#ifdef _EXPORTING
#define DECLSPEC __declspec(dllexport)
#else
#define DECLSPEC __declspec(dllimport)
#endif
#endif

typedef void AsyncReplyProcedure(AsyncID asyncId, uint64_t result);

/**
 * \class RPCClient
 * \brief Provides the user with a high abstraction level Remote Procedure Calls service.
 */
#ifdef WIN32
class	DECLSPEC RPCClient {
#else
class	RPCClient {
#endif
	Transport*				m_transportP;	// transport used to talk to the peer server
	RemoteProcedureCall* 	m_rpcP;			// rpc underlying abstraction
	mutex					m_asyncProcsMutex; 
	unordered_map<AsyncID, AsyncReplyProcedure*> m_async_procs;

public:
	/**
	 * \fn RPCClient(Transport::TransportType transport_type,  string &server_address)
	 * \brief RPCClient constructor
	 * 
	 * \param transport_type is one of the Transport::TransportType enum values
	 * \param server_address is the server address (for the given transport_type)
	*/
	RPCClient(Transport::TransportType transport_type,  const string& server_address) {
		m_rpcP = NULL;
		if (m_transportP = Transport::CreateTransport(transport_type)) {
			ReturnValue rv = m_transportP->LinkRequest((const string&)server_address);
			if (!rv.IsError()) {
				m_rpcP = new RemoteProcedureCall((Link*)rv);
			}
		}
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

	// to handle calls with variable number or arguments
	RpcReturnValue							  RpcCall(string func_name, ...);
	ReturnValue<AsyncID, CommunicationErrors> RpcCallAsync(AsyncReplyProcedure* procedureP, string func_name, ...);

	// to handle calls with ParameterBase* vector
	RpcReturnValue								RpcCall(string func_name, vector<RemoteProcedureCall::ParameterBase*>* paramsP);
	ReturnValue<AsyncID, CommunicationErrors>	RpcCallAsync(AsyncReplyProcedure* procedureP, string func_name, vector<RemoteProcedureCall::ParameterBase*>* paramsP);
};

#endif /* _RPC_CLIENT_H */
