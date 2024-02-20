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
 *		  WARNING : this is called asynchronously, all passed args
 *				    must remain valid for the whole execution time
 *					of THIS method.
 * \param rpc the reference of the instance of the remote procedure
			  call parameters (link, mutexes) valid for the whole
			  rpc client session.
 * \param procP points to the server's remote procedure call to invoke.
 * \param asyncId is the asynchronous call identifier, to be returned to
 * 		  the rpc client upon asynchronous remote procedure completion.
 *		  Its value is 0 for synchronous calls.
 * \param params is a shared ptr to the ServiceParameters, containing the
 *		  rpc server and communication link to the peer.
 * \param rpc_params is a shared ptr to the called procedure parameters 
 *		  vector.
 */

void RPCServer::CallServiceAndReply(RemoteProcedureCall& rpc, string& name, RemoteProcedure* procP, AsyncID asyncId, shared_ptr<ServiceParameters> params, shared_ptr<vector<RemoteProcedureCall::ParameterBase*>> rpc_params) {
	unsigned long result = (*procP)(name, rpc_params, params->m_serverP->m_user_dataP);
#ifdef RPCSERVER_TRACES
	LogVText(RPCSERVER_MODULE, 8, true, "CallServiceAndReply for asyncId %lu returned %lu", asyncId, result);
#endif

	// serialize back call results
	rpc.SerializeCallReturn(asyncId, rpc_params, result);
#ifdef RPCSERVER_TRACES
	LogText(RPCSERVER_MODULE, 8, true, "serialized call return");
#endif
}

void RPCServer::ServiceCallback(void* paramsP) {
	shared_ptr<ServiceParameters> params((ServiceParameters*)paramsP);

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
		string   		name;
#ifdef RPCSERVER_TRACES
		LogText(RPCSERVER_MODULE, 4, true, "waiting for incoming data...");
#endif

		// wait and deserialize call stream
		shared_ptr<vector<RemoteProcedureCall::ParameterBase*>> rpc_params(rpc.DeserializeCall(asyncId, name));
		if (!rpc_params.get() || rpc_params.get()->size() == 0) {
#ifdef RPCSERVER_TRACES
			LogVText(RPCSERVER_MODULE, 8, true, "server_error, couldn't deserialize parameters!");
#endif
			break;
		}

#ifdef RPCSERVER_TRACES
		LogVText(RPCSERVER_MODULE, 8, true, "deserialized into parameters vector: %p", rpc_params.get());
#endif

		// process rpc call
		RemoteProcedure* procP = params->m_serverP->m_rpc_map[name];
		if (!procP) {
			cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: unknown remote procedure name (" << name << ")!" << endl;
			// #### serialize back 'error'
			rpc.SerializeCallReturn(asyncId, rpc_params, 0);
			continue;
		}

#ifdef RPCSERVER_TRACES
		LogVText(RPCSERVER_MODULE, 8, true, "will call procedure %s, asyncId %lu", name.c_str(), asyncId);
#endif

		if (asyncId) {
			// asynchronous call
			Semaphore detachedSem(0);

			// create a thread which will wait for deserialized reply
			shared_ptr<thread> aThread = make_shared<thread>([&]() {
			  string _name = name;
				aThread->detach();
				detachedSem.R();
			

				CallServiceAndReply(rpc, _name, procP, asyncId, params, rpc_params);
			});

			detachedSem.A();
		} else {
			// synchronous call
			CallServiceAndReply(rpc, name, procP, asyncId, params, rpc_params);
		}
	}
}

