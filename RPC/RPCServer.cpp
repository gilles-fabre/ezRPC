#include <iostream>

#ifndef WIN32
#include <unistd.h>
#endif

#define _EXPORTING

#include "RPCServer.h"

/**
 * \fn void* RPCServer::ListeningCallback(void* _serverP)
 * \brief A single instance of listening thread call this method to
 *        wait for clients' requests to connect. Upon every new connection
 *        a link to the new client is established and passed over to a
 *        new dedicated service thread.
 *
 * \param _serverP is a pointer to the parent RPCServer
 */
void RPCServer::ListeningCallback(void* _serverP) {
	RPCServer* serverP = (RPCServer*)_serverP;
	
#ifdef RPCSERVER_TRACES
	LogVText(RPCSERVER_MODULE, 0, true, "RPCServer::listeningCallback(%p)", serverP);
#endif

	if (!serverP || !serverP->m_transportP)
		return;
#ifdef RPCSERVER_TRACES
	LogText(RPCSERVER_MODULE, 4, true, "waiting for link request");
#endif

	Link* linkP = serverP->m_transportP->WaitForLinkRequest(serverP->m_address);
	if (!linkP)
		return;

	Thread* threadP = new Thread(&ServiceCallback);
	serverP->m_serving_threads.push_back(threadP);
#ifdef RPCSERVER_TRACES
	LogText(RPCSERVER_MODULE, 4, true, "starting detached server thread...");
#endif

	threadP->Run((void*)new ServiceParameters(serverP, linkP));
}

/**
 * \fn void* RPCServer::ServiceCallback(void* _paramsP)
 * \brief A dedicated service thread, providing the linked client
 *        with remote procedures access. The procedures are those
 *        of the parent RPCServer.
 *
 * \param _paramsP is a pointer to a dedicated ServiceParameters, pointing
 *        to both the parent RPCServer and link to the client.
 */

void RPCServer::CallServiceAndReply(RemoteProcedureCall& rpc, RemoteProcedure* procP, AsyncID asyncId, const string& func_name, shared_ptr<ServiceParameters> params, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>> rpc_params) {
	shared_ptr<ServiceParameters> _params = params;
	shared_ptr<vector<RemoteProcedureCall::ParameterBase*>> _rpc_params = rpc_params;
	unsigned long result = (*procP)(_rpc_params.get(), _params->m_serverP->m_user_dataP);
#ifdef RPCSERVER_TRACES
	LogVText(RPCSERVER_MODULE, 8, true, "%s returned %lu", func_name.c_str(), result);
#endif

	// serialize back call results
	rpc.SerializeCallReturn(asyncId, _rpc_params.get(), result);
#ifdef RPCSERVER_TRACES
	LogText(RPCSERVER_MODULE, 8, true, "serialized call return");
#endif
}

void RPCServer::ServiceCallback(void* _paramsP) {
	shared_ptr<ServiceParameters> params((ServiceParameters*)_paramsP);

#ifdef RPCSERVER_TRACES
	LogVText(RPCSERVER_MODULE, 0, true, "RPCServer::ServiceCallback(%p)", params.get());
#endif

	if (!params.get() || !params->m_serverP || !params->m_linkP) {
		// can't talk to client
		return;
	}


#ifdef RPCSERVER_TRACES
	LogVText(RPCSERVER_MODULE, 4, true, "creating remote procedure call for server %p and link %p", params->m_serverP, params->m_linkP);
#endif

	RemoteProcedureCall rpc(params->m_linkP);
	for (;;) {
		AsyncID			asyncId;
		string   		func_name;
#ifdef RPCSERVER_TRACES
		LogText(RPCSERVER_MODULE, 4, true, "waiting for incoming data...");
#endif

		// wait and deserialize call stream
		shared_ptr<vector<RemoteProcedureCall::ParameterBase*>> rpc_params(rpc.DeserializeCall(asyncId, func_name));
		if (!rpc_params.get())
			break; 
#ifdef RPCSERVER_TRACES
		LogVText(RPCSERVER_MODULE, 8, true, "deserialized into parameters vector: %p", rpc_params.get());
#endif

		// process rpc call
		RemoteProcedure* procP = params->m_serverP->m_rpc_map[func_name];
		if (!procP) {
			cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: unknown remote procedure name (" << func_name << ")!" << endl;
			// serialize back 'error'
			rpc.SerializeCallReturn(asyncId, rpc_params.get(), 0);
			continue;
		}

#ifdef RPCSERVER_TRACES
		LogVText(RPCSERVER_MODULE, 8, true, "will call procedure %s, asyncId %lu", func_name.c_str(), asyncId);
#endif

		if (asyncId) {
			// asynchronous call
			Semaphore detachedSem(0);

			// create a thread which will wait for deserialized reply
			shared_ptr<thread> aThread = make_shared<thread>([&]() {
				aThread->detach();
				detachedSem.R();

				CallServiceAndReply(rpc, procP, asyncId, func_name, params, rpc_params);
			});

			detachedSem.A();
		} else {
			// synchronous call
			CallServiceAndReply(rpc, procP, asyncId, func_name, params, rpc_params);
		}
	}
}

