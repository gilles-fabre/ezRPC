#ifndef _RPC_CLIENT_H
#define _RPC_CLIENT_H

#include <vector>
#include <string>
#include <map>
#include <log2reporter.h>
#include <Transport.h>

#include "RemoteProcedureCall.h"

using namespace std;

#define RPCCLIENT_TRACES 1
#define RPCCLIENT_MODULE 0x5

/**
 * \class RPCClient
 * \brief Provides the user with a high abstraction level Remote Procedure Calls service.
 */
class RPCClient {
	Transport 		 	*m_transportP; 	// transport used to talk to the peer server
	RemoteProcedureCall *m_rpcP;		// rpc underlying abstraction

public:
	RPCClient(Transport::TransportType transport_type,  string &server_address) {
		m_transportP = Transport::CreateTransport(transport_type);
		Link *linkP = m_transportP ? m_transportP->LinkRequest((const string &)server_address) : NULL;
		m_rpcP = linkP ? new RemoteProcedureCall(linkP) : NULL;
	}

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

	void Stop() {
		if (m_transportP)
			m_transportP->Close();
	}

	unsigned long RpcCall(string func_name, ...);
};

#endif /* _RPC_CLIENT_H */
