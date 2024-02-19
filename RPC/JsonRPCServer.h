#ifndef _JSON_RPC_SERVER_H
#define _JSON_RPC_SERVER_H

#include <unordered_map>
#include <log2reporter.h>
#include <RPCServer.h>

using namespace std;

#ifdef _DEBUG
#define JSONRPCSERVER_TRACES 1
#endif

#define JSONRPCSERVER_MODULE 0x20

#ifdef WIN32
#pragma warning(disable:4251)
#ifdef _EXPORTING
#define DECLSPEC __declspec(dllexport)
#else
#define DECLSPEC __declspec(dllimport)
#endif
#endif

/**
 * \JsonRPCServer C API
 * \brief Provides C APIs on top of RPCServer services
 */

typedef uint64_t ServerProcedure(const char* jsonCall, char* jsonCallResult, size_t jsonCallResultLen);

extern "C" {
DECLSPEC uint64_t CreateRpcServer(Transport::TransportType transport, const char *serverAddrP);
DECLSPEC void	  RegisterProcedure(uint64_t serverId, const char* name, ServerProcedure* procedureP);
DECLSPEC void	  DestroyRpcServer(uint64_t serverId);
};

/**
 * \class JsonRPCServer
 * \brief Provides the user with a high abstraction level Remote Procedure Calls server.
 */
#ifdef WIN32
class	DECLSPEC JsonRPCServer {
#else
class	JsonRPCServer {
#endif
	unique_ptr<RPCServer>	m_server;

	static mutex									m_serverProcsMutex;
	static unordered_map<string, ServerProcedure*>	m_serverProcs;

public:
	JsonRPCServer(Transport::TransportType transport, const string& serverAddr);

	static unsigned long JsonRPCServiceProc(string& name, vector<RemoteProcedureCall::ParameterBase*>*, void* user_dataP);
};

#endif /* _JSON_RPC_SERVER_H */
#pragma once
