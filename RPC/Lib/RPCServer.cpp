#include <iostream>

#ifndef WIN32
#include <unistd.h>
#endif

#include "RPCServer.h"

/**
 * \fn void *RPCServer::ListeningCallback(void *_serverP)
 * \brief A single instance of listening thread call this method to
 *        wait for clients' requests to connect. Upon every new connection
 *        a link to the new client is established and passed over to a
 *        new dedicated service thread.
 *
 * \param _serverP is a pointer to the parent RPCServer
 */
void RPCServer::ListeningCallback(void *_serverP) {
	RPCServer *serverP = (RPCServer *)_serverP;
#ifdef RPCSERVER_TRACES
	LogVText(RPCSERVER_MODULE, 0, true, "RPCServer::listeningCallback(%p)", serverP);
#endif

	if (!serverP || !serverP->m_transportP)
		return;
#ifdef RPCSERVER_TRACES
	LogText(RPCSERVER_MODULE, 4, true, "waiting for link request");
#endif

	Link *linkP = serverP->m_transportP->WaitForLinkRequest(serverP->m_address);
	if (!linkP)
		return;

	Thread *threadP = new Thread(&ServiceCallback);
	serverP->m_serving_threads.push_back(threadP);
#ifdef RPCSERVER_TRACES
	LogText(RPCSERVER_MODULE, 4, true, "starting detached server thread...");
#endif

	threadP->Run((void *)new ServiceParameters(serverP, linkP));
}

/**
 * \fn void *RPCServer::ServiceCallback(void *_paramsP)
 * \brief A dedicated service thread, providing the linked client
 *        with remote procedures access. The procedures are those
 *        of the parent RPCServer.
 *
 * \param _paramsP is a pointer to a dedicated ServiceParameters, pointing
 *        to both the parent RPCServer and link to the client.
 */
void RPCServer::ServiceCallback(void *_paramsP) {
	ServiceParameters *paramsP = (ServiceParameters *)_paramsP;
#ifdef RPCSERVER_TRACES
	LogVText(RPCSERVER_MODULE, 0, true, "RPCServer::ServiceCallback(%p)", paramsP);
#endif

	// ?!
	if (!paramsP)
		return;

	if (!paramsP->m_serverP || !paramsP->m_linkP) {
		// can't talk to client
		delete paramsP;
		return;
	}


#ifdef RPCSERVER_TRACES
	LogVText(RPCSERVER_MODULE, 4, true, "creating remote procedure call for server %p and link %p", paramsP->m_serverP, paramsP->m_linkP);
#endif

	RemoteProcedureCall rpc(paramsP->m_linkP);
	for (;;) {
		unsigned long 	result;
		string   		func_name;
#ifdef RPCSERVER_TRACES
		LogText(RPCSERVER_MODULE, 4, true, "waiting for incoming data...");
#endif

		// wait and deserialize call stream
		vector<RemoteProcedureCall::Parameter *> *rpc_paramsP = rpc.DeserializeCall(func_name);
		if (!rpc_paramsP)
			break; 
#ifdef RPCSERVER_TRACES
		LogVText(RPCSERVER_MODULE, 8, true, "deserialized into parameters vector: %p", rpc_paramsP);
#endif

		// process rpc call
		RemoteProcedure *procP = paramsP->m_serverP->m_rpc_map[func_name];
		if (!procP) {
			cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: unknown remote procedure name!" << endl;
			// serialize back 'error'
			rpc.SerializeCallReturn(rpc_paramsP, 0);
			continue;
		}
#ifdef RPCSERVER_TRACES
		LogVText(RPCSERVER_MODULE, 8, true, "will call procedure %s", func_name.c_str());
#endif

		result = (*procP)(rpc_paramsP, paramsP->m_serverP->m_user_dataP);
#ifdef RPCSERVER_TRACES
		LogVText(RPCSERVER_MODULE, 8, true, "%s returned %lu", func_name.c_str(), result);
#endif

		// serialize back call results
		rpc.SerializeCallReturn(rpc_paramsP, result);
#ifdef RPCSERVER_TRACES
		LogText(RPCSERVER_MODULE, 8, true, "serialized call return");
#endif

		// done with these parameters
		for (vector<RemoteProcedureCall::Parameter *>::iterator i = rpc_paramsP->begin(); i != rpc_paramsP->end(); i++)
			delete *i;
		delete rpc_paramsP;
#ifdef RPCSERVER_TRACES
		LogText(RPCSERVER_MODULE, 8, true, "cleaned up parameters");
#endif
	}

	delete paramsP;
#ifdef RPCSERVER_TRACES
	LogText(RPCSERVER_MODULE, 8, true, "cleaned up service callback parameters, exiting");
#endif
}

