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

#include <signal.h>

#include <iostream>
#include <string>

#include <StateMachine.h>

#include "EPE.h"

ezProtoEngine *g_epeP = NULL;

map<string, RPCClient *> ezProtoEngine::m_clients;		// the clients' rpcs

/**
 * \fn unsigned long ezProtoEngine::RegisterEClient(vector<RemoteProcedureCall::Parameter *> *v)
 * \brief Registers an EClient's RPCServer against the EPE. This enables the EPE to callback into a
 *        registered EClient's RPCServer's procedures.
 *
 * \param v is a vector of Remote Procedure Call Parameters, which contains the following
 *        elements:
 *        	result return address : unsigned long (unused, do not modify)
 *        	client address : string, uniquely identifies the EClient address on the transport
 *			transport type : TCP or FILE
 *
 * \return 0 if everything went fine, else return -1
 */
unsigned long ezProtoEngine::RegisterEClient(vector<RemoteProcedureCall::Parameter *> *v) {
	// check params
	if (v->size() != 3)
		return (unsigned long)-1;

	RemoteProcedureCall::Parameter *transport_typeP = (*v)[1];  // transport type
	RemoteProcedureCall::Parameter *client_addressP = (*v)[2];	// client's rpc server address

#ifdef EPE_TRACES
	cout << "\n ezProtoEngine::RegisterEClient(" << v << ")" << endl;
#endif

	// check parameters
	string &client_address = client_addressP->GetStringValue();
	if (client_address.empty() || m_clients[client_address])
		return (unsigned long)-1;

	// client callbacks are implicit....
	// but we need to talk to the client rpcservers (for callbacks)
	RPCClient *rpcP = new RPCClient((Transport::TransportType)transport_typeP->GetByteValue(), client_address);
	if (rpcP)
		m_clients[client_address] = rpcP;

#ifdef EPE_TRACES
	cout << "\t registered client: " << client_address << endl;
#endif

	return 0;
}

/**
 * \fn unsigned long ezProtoEngine::UnregisterEClient(vector<RemoteProcedureCall::Parameter *> *v)
 * \brief Unregisters an EClient's RPCServer.
 *
 * \param v is a vector of Remote Procedure Call Parameters, which contains the following
 *        elements:
 *        	result return address : unsigned long (unused, do not modify)
 *        	client address : string, uniquely identifies the EClient address on the transport
 *
 * \return 0 if everything went fine, else return -1
 */
unsigned long ezProtoEngine::UnregisterEClient(vector<RemoteProcedureCall::Parameter *> *v) {
	// check params
	if (v->size() != 2)
		return (unsigned long)-1;

	RemoteProcedureCall::Parameter *client_addressP = (*v)[1];	// client's rpc server address

#ifdef EPE_TRACES
	cout << "\n ezProtoEngine::UnregisterEClient(" << v << ")" << endl;
#endif

	// check parameters
	string &client_address = client_addressP->GetStringValue();
	if (client_address.empty() || !m_clients[client_address])
		return (unsigned long)-1;

	delete m_clients[client_address];
	m_clients.erase(client_address);

#ifdef EPE_TRACES
	cout << "\t unregistered client: " << client_address << endl;
#endif

	return 0;
}

/**
 * \fn unsigned long ezProtoEngine::CreateStateMachine(vector<RemoteProcedureCall::Parameter *> *v)
 * \brief Creates a state machine based on the given json definition file
 *
 * \param v is a vector of Remote Procedure Call Parameters, which contains the following
 *        elements:
 *        	result return address : unsigned long (unused, do not modify)
 *  		client address : the client's rpcserver address
 *  		user data pointer : the user data pointer associated with the state machine (is passed
 *  						    to all transition callbacks
 *
 * \return the newly created state machine address or NULL if an error occurred
 */
unsigned long ezProtoEngine::CreateStateMachine(vector<RemoteProcedureCall::Parameter *> *v) {
	// check params
	if (v->size() != 4)
		return 0;

	RemoteProcedureCall::Parameter *client_addressP = (*v)[1];	// client's rpc server address
	RemoteProcedureCall::Parameter *json_filenameP = (*v)[2];	// json state machine definition filename
	RemoteProcedureCall::Parameter *user_data = (*v)[3];		// user data pointer to pass to the state machine

	string 		client_address = client_addressP->GetStringValue();
	string 		json_filename = json_filenameP->GetStringValue();
	void		*user_dataP = (void *)user_data->GetUInt64Value();

	if (client_address.empty() || json_filename.empty())
		return 0;

	// find client from address.
	RPCClient *rpcP = m_clients[client_address];
	if (!rpcP)
		return 0;

	StateMachine *smP = StateMachine::CreateFromDefinition(json_filename);
	if (!smP)
		return 0;

	if (user_dataP)
		smP->SetUserData(user_dataP);

	// so the SM can callback the client
	smP->SetRpcClient(rpcP);

	return (uint64_t)smP;
}

/**
 * \fn unsigned long ezProtoEngine::DestroyStateMachine(vector<RemoteProcedureCall::Parameter *> *v)
 * \brief Destroys the given state machine
 *
 * \param v is a vector of Remote Procedure Call Parameters, which contains the following
 *        elements:
 *        	result return address : unsigned long (unused, do not modify)
 *  		client address : the client's rpcserver address
 *  		state machine address : the address of the state machine to release
 *
 * \return 0 or (unsigned long)-1 if an error occurred
 */
unsigned long ezProtoEngine::DestroyStateMachine(vector<RemoteProcedureCall::Parameter *> *v) {
	// check params
	if (v->size() != 3)
		return (unsigned long)-1;

	RemoteProcedureCall::Parameter *client_addressP = (*v)[1];	// client's rpc server address
	RemoteProcedureCall::Parameter *smidP = (*v)[2];			// state machine id

	string 			client_address = client_addressP->GetStringValue();
	StateMachine 	*smP = (StateMachine *)smidP->GetUInt64Value();

	if (client_address.empty() || !smP)
		return (unsigned long)-1;

	// ask the SM to delete itself
	smP->Release();

	return 0;
}

/**
 * \fn unsigned long ezProtoEngine::DoTransition(vector<RemoteProcedureCall::Parameter *> *v)
 * \brief Commands the given state machine to do the required transition
 *
 * \param v is a vector of Remote Procedure Call Parameters, which contains the following
 *        elements:
 *        	result return address : unsigned long (unused, do not modify)
 *  		state machine address : the address of the state machine to release
 *  		transition name : the name of the required transition
 *
 * \return 0 or (unsigned long)-1 if an error occurred
 */
unsigned long ezProtoEngine::DoTransition(vector<RemoteProcedureCall::Parameter *> *v) {
	// check params
	if (v->size() != 4)
		return (unsigned long)-1;

	RemoteProcedureCall::Parameter *client_addressP = (*v)[1];	// client's rpc server address
	RemoteProcedureCall::Parameter *smidP = (*v)[2];			// state machine id
	RemoteProcedureCall::Parameter *transitionP = (*v)[3];		// transition name

	string 			client_address = client_addressP->GetStringValue();
	string 			transition = transitionP->GetStringValue();
	StateMachine 	*smP = (StateMachine *)smidP->GetUInt64Value();

	if (client_address.empty() || transition.empty() || !smP)
		return (unsigned long)-1;

	// so the SM can callback the client
	smP->DoTransition(transition);

	return 0;
}

/**
 * \fn void signal_callback_handler(int signum)
 * \brief handles program termination by signal
 *
 * \param signum is the raised signal id
 */
void signal_callback_handler(int signum) {
	   cout << "Caught signal " << signum << ", now cleaning up and exiting..." << endl;

	   // cleanup
	   if (g_epeP)
		   g_epeP->Stop();

	   // terminate program
	   exit(signum);
}

int main(int argc, char **argv) {
   // register signal and signal handler
   signal(SIGINT, signal_callback_handler);

	if (argc < 3) {
		cout << "usage:" << endl;
		cout << "\tEPE <tcp|file> server_address" << endl;
		return -1;
	}

	Transport::TransportType 	type;
	string 						type_string = argv[1];
	string 						server_address = argv[2];

	if (type_string == "tcp")
		type = Transport::TCP;
	else if (type_string == "file")
		type = Transport::FILE;
	else {
		cout << "usage:" << endl;
		cout << "\tEPE <tcp|file> server_address" << endl;
		return -1;
	}

	// #### add exceptions
	ezProtoEngine epe(type, server_address);
	g_epeP = &epe;
	epe.Run();

	return 0;
}
