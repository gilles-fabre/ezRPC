#ifndef _JSON_RPC_SERVER_H
#define _JSON_RPC_SERVER_H

#include <unordered_map>
#include <log2reporter.h>
#include <RPCServer.h>

//using namespace std;

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

#define JSON_RESULT_MAX_SIZE 102400

/**
 * \JsonRPCServer C API
 * \brief Provides C APIs on top of RPCServer services
 */

typedef uint64_t ServerProcedure(AsyncID asyncId, const char* jsonCallP, char* jsonCallResultP, size_t jsonCallResultLen);

extern "C" {
#ifdef WIN32
DECLSPEC 
#endif
uint64_t CreateRpcServer(Transport::TransportType transport, const char *serverAddrP);
#ifdef WIN32
DECLSPEC 
#endif
void	  RegisterProcedure(uint64_t serverId, const char* nameP, ServerProcedure* procedureP);
#ifdef WIN32
DECLSPEC 
#endif
void	  UnregisterProcedure(uint64_t serverId, const char* nameP);
#ifdef WIN32
DECLSPEC 
#endif
void	  DestroyRpcServer(uint64_t serverId);
#ifdef WIN32
DECLSPEC 
#endif
void     IterateAndWait(uint64_t serverId);
#ifdef WIN32
DECLSPEC 
#endif
void     Stop(uint64_t serverId);
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
public:
	unique_ptr<RPCServer>	m_server;
	static mutex									m_serverProcsMutex;
	static unordered_map<string, ServerProcedure*>	m_serverProcs;

	JsonRPCServer(Transport::TransportType transport, const string& serverAddr);

	static RpcReturnValue JsonRPCServiceProc(AsyncID asyncId, const string& name, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>> params, void* user_dataP);
};

#endif /* _JSON_RPC_SERVER_H */
#pragma once
