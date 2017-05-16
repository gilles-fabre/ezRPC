/*
                     GNU LESSER GENERAL PUBLIC LICENSE
                       Version 3, 29 June 2007

 Copyright (C) 2007 Free Software Foundation, Inc. <http://fsf.org/>
 Everyone is permitted to copy and distribute verbatim copies
 of this license document, but changing it is not allowed.


  This version of the GNU Lesser General Public License incorporates
the terms and conditions of version 3 of the GNU General Public
License, supplemented by the additional permissions listed below.

  0. Additional Definitions.

  As used herein, "this License" refers to version 3 of the GNU Lesser
General Public License, and the "GNU GPL" refers to version 3 of the GNU
General Public License.

  "The Library" refers to a covered work governed by this License,
other than an Application or a Combined Work as defined below.

  An "Application" is any work that makes use of an interface provided
by the Library, but which is not otherwise based on the Library.
Defining a subclass of a class defined by the Library is deemed a mode
of using an interface provided by the Library.

  A "Combined Work" is a work produced by combining or linking an
Application with the Library.  The particular version of the Library
with which the Combined Work was made is also called the "Linked
Version".

  The "Minimal Corresponding Source" for a Combined Work means the
Corresponding Source for the Combined Work, excluding any source code
for portions of the Combined Work that, considered in isolation, are
based on the Application, and not on the Linked Version.

  The "Corresponding Application Code" for a Combined Work means the
object code and/or source code for the Application, including any data
and utility programs needed for reproducing the Combined Work from the
Application, but excluding the System Libraries of the Combined Work.

  1. Exception to Section 3 of the GNU GPL.

  You may convey a covered work under sections 3 and 4 of this License
without being bound by section 3 of the GNU GPL.

  2. Conveying Modified Versions.

  If you modify a copy of the Library, and, in your modifications, a
facility refers to a function or data to be supplied by an Application
that uses the facility (other than as an argument passed when the
facility is invoked), then you may convey a copy of the modified
version:

   a) under this License, provided that you make a good faith effort to
   ensure that, in the event an Application does not supply the
   function or data, the facility still operates, and performs
   whatever part of its purpose remains meaningful, or

   b) under the GNU GPL, with none of the additional permissions of
   this License applicable to that copy.

  3. Object Code Incorporating Material from Library Header Files.

  The object code form of an Application may incorporate material from
a header file that is part of the Library.  You may convey such object
code under terms of your choice, provided that, if the incorporated
material is not limited to numerical parameters, data structure
layouts and accessors, or small macros, inline functions and templates
(ten or fewer lines in length), you do both of the following:

   a) Give prominent notice with each copy of the object code that the
   Library is used in it and that the Library and its use are
   covered by this License.

   b) Accompany the object code with a copy of the GNU GPL and this license
   document.

  4. Combined Works.

  You may convey a Combined Work under terms of your choice that,
taken together, effectively do not restrict modification of the
portions of the Library contained in the Combined Work and reverse
engineering for debugging such modifications, if you also do each of
the following:

   a) Give prominent notice with each copy of the Combined Work that
   the Library is used in it and that the Library and its use are
   covered by this License.

   b) Accompany the Combined Work with a copy of the GNU GPL and this license
   document.

   c) For a Combined Work that displays copyright notices during
   execution, include the copyright notice for the Library among
   these notices, as well as a reference directing the user to the
   copies of the GNU GPL and this license document.

   d) Do one of the following:

       0) Convey the Minimal Corresponding Source under the terms of this
       License, and the Corresponding Application Code in a form
       suitable for, and under terms that permit, the user to
       recombine or relink the Application with a modified version of
       the Linked Version to produce a modified Combined Work, in the
       manner specified by section 6 of the GNU GPL for conveying
       Corresponding Source.

       1) Use a suitable shared library mechanism for linking with the
       Library.  A suitable mechanism is one that (a) uses at run time
       a copy of the Library already present on the user's computer
       system, and (b) will operate properly with a modified version
       of the Library that is interface-compatible with the Linked
       Version.

   e) Provide Installation Information, but only if you would otherwise
   be required to provide such information under section 6 of the
   GNU GPL, and only to the extent that such information is
   necessary to install and execute a modified version of the
   Combined Work produced by recombining or relinking the
   Application with a modified version of the Linked Version. (If
   you use option 4d0, the Installation Information must accompany
   the Minimal Corresponding Source and Corresponding Application
   Code. If you use option 4d1, you must provide the Installation
   Information in the manner specified by section 6 of the GNU GPL
   for conveying Corresponding Source.)

  5. Combined Libraries.

  You may place library facilities that are a work based on the
Library side by side in a single library together with other library
facilities that are not Applications and are not covered by this
License, and convey such a combined library under terms of your
choice, if you do both of the following:

   a) Accompany the combined library with a copy of the same work based
   on the Library, uncombined with any other library facilities,
   conveyed under the terms of this License.

   b) Give prominent notice with the combined library that part of it
   is a work based on the Library, and explaining where to find the
   accompanying uncombined form of the same work.

  6. Revised Versions of the GNU Lesser General Public License.

  The Free Software Foundation may publish revised and/or new versions
of the GNU Lesser General Public License from time to time. Such new
versions will be similar in spirit to the present version, but may
differ in detail to address new problems or concerns.

  Each version is given a distinguishing version number. If the
Library as you received it specifies that a certain numbered version
of the GNU Lesser General Public License "or any later version"
applies to it, you have the option of following the terms and
conditions either of that published version or of any later version
published by the Free Software Foundation. If the Library as you
received it does not specify a version number of the GNU Lesser
General Public License, you may choose any version of the GNU Lesser
General Public License ever published by the Free Software Foundation.

  If the Library as you received it specifies that a proxy can decide
whether future versions of the GNU Lesser General Public License shall
apply, that proxy's public statement of acceptance of any version is
permanent authorization for you to choose that version for the
Library.
*/

/**
 * \file RPCServer.cpp
 *
 * \author gilles-fabre
 * \date Mar 21, 2017
 */

#include <iostream>
#include <unistd.h>

#include "RPCServer.h"

/**
 * \fn void *RPCServer::ListeningCallback(void *_serverP)
 * \brief A single instance of listening thread call this method to
 *        wait for clients' requests to connect. Upon every new connection
 *        a link to the new client is established and passed over to a
 *        new dedicated service thread.
 *
 * \param _serverP is a pointer to the parent RPCServer
 * \return NULL
 */
void *RPCServer::ListeningCallback(void *_serverP) {
	RPCServer *serverP = (RPCServer *)_serverP;
#ifdef RPCSERVER_TRACES
	cout << "\n RPCServer::ListeningCallback(" << serverP << ")" << endl;
#endif

	if (!serverP || !serverP->m_transportP)
		return NULL;
#ifdef RPCSERVER_TRACES
	cout << "\t waiting for link request..." << endl;
#endif

	Link *linkP = serverP->m_transportP->WaitForLinkRequest(serverP->m_address);
	if (!linkP)
		return NULL;

	Thread *threadP = new Thread(&ServiceCallback);
	serverP->m_serving_threads.push_back(threadP);
#ifdef RPCSERVER_TRACES
	cout << "\t starting detached service thread..." << endl;
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
 * \return NULL
 */
void *RPCServer::ServiceCallback(void *_paramsP) {
	ServiceParameters *paramsP = (ServiceParameters *)_paramsP;
#ifdef RPCSERVER_TRACES
	cout << "\n RPCServer::ServiceCallback(" << paramsP << ")" << endl;
#endif

	// ?!
	if (!paramsP)
		return NULL;

	if (!paramsP->m_serverP || !paramsP->m_linkP) {
		// can't talk to client
		delete paramsP;
		return NULL;
	}


#ifdef RPCSERVER_TRACES
	cout << "\t creating remote procedure call for link: " << paramsP->m_linkP << endl;
#endif

	RemoteProcedureCall rpc(paramsP->m_linkP);
	for (;;) {
		unsigned long 	result;
		string   		func_name;
#ifdef RPCSERVER_TRACES
		cout << "\t\t waiting for incoming data..." << endl;
#endif

		// wait and deserialize call stream
		vector<RemoteProcedureCall::Parameter *> *rpc_paramsP = rpc.DeserializeCall(func_name);
		if (!rpc_paramsP)
			break; 
#ifdef RPCSERVER_TRACES
		cout << "\t\t deserialized into parameters vector: " << rpc_paramsP << endl;
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
		cout << "\t\t will call procedure " << func_name << endl;
#endif

		result = (*procP)(rpc_paramsP);
#ifdef RPCSERVER_TRACES
		cout << "\t\t " << func_name << " returned: " << result << endl;
#endif

		// serialize back call results
		rpc.SerializeCallReturn(rpc_paramsP, result);
#ifdef RPCSERVER_TRACES
		cout << "\t\t serialized call return" << endl;
#endif

		// done with these parameters
		for (vector<RemoteProcedureCall::Parameter *>::iterator i = rpc_paramsP->begin(); i != rpc_paramsP->end(); i++)
			delete *i;
		delete rpc_paramsP;
#ifdef RPCSERVER_TRACES
		cout << "\t\t cleaned up parameters" << endl;
#endif
	}

	delete paramsP;
#ifdef RPCSERVER_TRACES
	cout << "\t cleaned up service callback parameters, exiting" << endl;
#endif

	// if the link is broken, close this side of the socket
	if (!rpc.IsConnected())
		rpc.Close();
}

