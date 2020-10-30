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
 * \file StateMachine.cpp
 *
 * \author  gilles fabre
 * \date Mar 14, 2017
 */

#include <stdio.h>
#include <unistd.h>
#include <json/json.h>

#include <iostream>
#include <fstream>

#include "StateMachine.h"

Thread		  	 		StateMachine::m_garbage_collector_thread(&GarbageCollectorCallback);
list<StateMachine *>	StateMachine::m_machines;

/**
 * \fn StateMachine(void *pUserData)
 * \brief StateMachine constructor, WARNING: does not set the initial state.
 *
 * \param name is the name of the state machine
 * \param user_dataP is a pointer to a user data block which'll be passed to all
 *        transition callbacks
 */
StateMachine::StateMachine(string &name, void *user_dataP) :
		m_call_sem(1),
		m_exiting(0),
		m_timer_sem(1),
		m_timer_thread(&TimerThreadCallback) {
	m_rpcClientP = NULL;
	m_name = name;
	m_user_dataP = user_dataP;
	m_current_stateP = NULL; // one must set this explicitly once the initial state is created

	// insert this state machine into the list
	m_machines.push_back(this);

	// create the garbage collector thread if not done yet
	if (!m_garbage_collector_thread.IsRunning())
		// start the garbage collector thread
		m_garbage_collector_thread.Run(NULL);
}

/**
 * \fn Release()
 * \brief Stops the state machine's context pending threads, then mark the state machine as
 *        exiting so the garbage collector thread can destroy it.
 */
void StateMachine::Release() {
	// we're exiting
	m_exiting.Inc();

	// do not further generate calls from timer thread into this machine
	CancelTransitionTimer();
}

/**
 * \fn ~StateMachine()
 * \brief StateMachine destructor, clear all of the State Machine content. DO NOT
 *        CALL DIRECTLY. Call Release instead. The Garbage collector will free this
 *        SM later on when its safe (no pending threads)
 *
 */
StateMachine::~StateMachine() {
#ifdef STATE_MACHINE_TRACES
	cout << "State Machine " << hex << "0x" << this << dec << " is being destroyed" << endl;
#endif

	// don't start killing anything until we're clear with all the
	// children threads...

	// delete all the states
	m_call_sem.A();
	for (map<string, State *>::iterator i = m_states.begin(); i != m_states.end(); i++)
		delete (*i).second;
	m_call_sem.R();
}

/**
 * \fn GarbageCollector
 * \brief StateMachine garbage collector destroys the exiting state machines
 * 		  every once in a while, ensuring all pending callback threads have
 * 		  triggered or have been cancelled before then.
 *
 */
void *StateMachine::GarbageCollectorCallback(void *parametersP) {
	bool exit = false;

#ifdef STATE_MACHINE_TRACES
	cout << "State Machine Garbage Collector is starting" << endl;
#endif

	while (!exit) {
		sleep(GARBAGE_COLLECTOR_PAUSE);

		// go through all the state machines and delete those that are declared as exiting
		list<StateMachine *>::iterator i = m_machines.begin();
		while (i != m_machines.end()) {
			StateMachine *smP = *i++;

			if (smP->IsExiting() && !smP->IsPendingThread()) {
#ifdef STATE_MACHINE_TRACES
				cout << "State Machine Garbage Collector is deleting state machine " << smP << endl;
#endif
				m_machines.remove(smP);
				delete smP;
			}
		}

		if (m_machines.empty())
			exit = true;
	}

#ifdef STATE_MACHINE_TRACES
	cout << "State Machine Garbage Collector is exiting" << endl;
#endif

	return NULL;
}

/**
 * \fn void StateMachine::CallbackThread(void *parametersP)
 * \brief Creates and starts a new thread to call a transition callback
 *
 * \param parametersP points to a CallbackThreadParameters structure
 */
void *StateMachine::CallbackThread(void *parametersP) {
	CallbackThreadParameters *paramsP = (CallbackThreadParameters *)parametersP;
	if (!paramsP || !paramsP->m_machineP || paramsP->m_machineP->IsExiting()) {
		paramsP->m_machineP->m_callback_threads.remove(paramsP->m_threadP);
		return NULL;
	}

#ifdef STATE_MACHINE_TRACES
	cout << "CallbackThread launching callback for transition " << paramsP->m_transition_name << endl;
#endif

	if (!paramsP->m_machineP->IsExiting()) {
		// invoke remote procedure...
		RPCClient *clientP = (RPCClient *)paramsP->m_machineP->GetRpcClient();
		if (clientP) {
			string machine = paramsP->m_machineP->GetName();
			string src = paramsP->m_sourceP->GetName();
			string dst = paramsP->m_destinationP->GetName();
			string function_name = paramsP->m_callback_name.empty() ? "TransitionCallback" : paramsP->m_callback_name;
			clientP->RpcCall(function_name,
							 RemoteProcedureCall::UINT64,
							 (uint64_t)paramsP->m_machineP,
							 RemoteProcedureCall::STRING,
							 &machine,
							 RemoteProcedureCall::STRING,
							 &src,
							 RemoteProcedureCall::STRING,
							 &paramsP->m_transition_name,
							 RemoteProcedureCall::STRING,
							 &dst,
							 RemoteProcedureCall::UINT64,
							 (uint64_t)paramsP->m_user_dataP,
							 RemoteProcedureCall::END_OF_CALL);
		}
	}

	paramsP->m_machineP->m_callback_threads.remove(paramsP->m_threadP);
	return NULL;
}

/**
 * \fn void *StateMachine::InitiateTransitionTimer(unsigned long milli_sec_timeout, string &transition_name)
 * \brief Creates and starts a timer which will inject a transition into the current state when
 *        the given time has elapsed.
 *
 * \param milli_sec_timeout is the delay before the transition is injected in the state machine
 * \param transition_name is the injected transition's identifier
 */
void *StateMachine::TimerThreadCallback(void *parametersP) {
	TimerThreadParameters *paramsP = (TimerThreadParameters *)parametersP;

	if (!paramsP || !paramsP->m_machineP)
		return NULL;

#ifdef STATE_MACHINE_TRACES
	cout << "TimerThread sleeping for " << paramsP->m_milli_seconds << " millisecond(s)" << endl;
#endif

	// if 0 returned, the cancelled semaphore was inc'ed by the state machine
	if (paramsP->m_cancelled.WaitA(paramsP->m_milli_seconds)) {

		paramsP->m_machineP->m_timer_sem.A();

		if (!paramsP->m_machineP->m_timer_thread.IsAskedToStop()) {
			#ifdef STATE_MACHINE_TRACES
			cout << "TimerThread TIMEOUT!!!" << endl;
			#endif
			paramsP->m_machineP->m_timer_sem.R();
			paramsP->m_machineP->DoTransition(paramsP->m_transition_name, true);
		} else
			paramsP->m_machineP->m_timer_sem.R();
	}
#ifdef STATE_MACHINE_TRACES
		else cout << "TimerThread Cancelled!!!" << endl;
#endif

	return NULL;
}

void StateMachine::InitiateTransitionTimer(unsigned long milli_sec_timeout, const string &transition_name) {
	// cancel pending timer if any
	CancelTransitionTimer();

	m_timer_sem.A();

	m_timer_thread.Run(new TimerThreadParameters(this, milli_sec_timeout, transition_name));

	m_timer_sem.R();
}

/**
 * \fn StateMachine *StateMachine::CreateFromDefinition(StateMachineDefinition *pDef)
 * \brief This factory method creates a state machine from the given definition.
 *
 * \param json_defintion_filename is the JSON SM definition file.
 * \param id is the newly created state machine id (can be retrieved later on to identify
 * 		  the machine.
 *
 * \return the created machine or NULL if an error occured.
 */
StateMachine *StateMachine::CreateFromDefinition(string &json_definition_filename) {
	Json::Value 	root;   		// 'root' will contain the root value after parsing.
	StateMachine	*smP = NULL;

#ifdef STATE_MACHINE_TRACES
	cout << "\n StateMachine::CreateFromDefinition(" << json_definition_filename << ")" << endl;
#endif

	try {
		ifstream file(json_definition_filename);
		if (!file.is_open()) {
			cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: couldn't open " << json_definition_filename << "!" << endl;
			return NULL;
		}

		file >> root;
#ifdef STATE_MACHINE_TRACES
		cout << "\t JSON result:" << endl << root << endl;
#endif

		// create the state machine from the file definition

		// get and check state machine object
		Json::Value sm = root["StateMachine"];
		if (sm.empty() || !sm.isObject()) {
			cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: JSON file missing 'StateMachine' object!" << endl;
			return NULL;
		}

		// get and check machine's name
		if (!sm.isMember("name") || !sm["name"].isString()) {
			cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: StateMachine definition missing 'name' object!" << endl;
			return NULL;
		}
		string machine_name = sm["name"].asString();
		if (machine_name.empty()) {
			cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: StateMachine's 'name' is empty!" << endl;
			return NULL;
		}

		// get and check machine's initial state
		if (!sm.isMember("initial_state") || !sm["initial_state"].isString()) {
			cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: StateMachine definition missig 'initial_state' object!" << endl;
			return NULL;
		}
		string initial_state = sm["initial_state"].asString();
		if (initial_state.empty()) {
			cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: StateMachine's 'initial_state' is empty!" << endl;
			return NULL;
		}

		// get and check states
		if (!sm.isMember("states") || !sm["states"].isArray()) {
			cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: StateMachine definition missing 'states' array!" << endl;
			return NULL;
		}
		Json::Value states = sm["states"];
		for (Json::Value::iterator state = states.begin(); state !=  states.end(); state++) {
			if (!(*state).isString()) {
				cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: StateMachine's 'states' array must contain strings only!" << endl;
				return NULL;
			}
		}

		// get and check transitions
		if (!sm.isMember("transitions") || !sm["transitions"].isArray()) {
			cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: StateMachine definition missing 'transitions' array!" << endl;
			return NULL;
		}
		Json::Value transitions = sm["transitions"];
		for (Json::Value::iterator transition = transitions.begin(); transition != transitions.end(); transition++) {
			Json::Value &t = *transition;
			if (!t.isObject()) {
				cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: StateMachine's 'transitions' array must contain transition objects only!" << endl;
				return NULL;
			}

			if (!t.isMember("name") || !t["name"].isString()) {
				cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: " << t << " is missing transition 'name'!" << endl;
				return NULL;
			}

			if (!t.isMember("source") || !t["source"].isString()) {
				cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: " << t << " is missing 'source' state name!" << endl;
				return NULL;
			}

			if (!t.isMember("destination") || !t["destination"].isString()) {
				cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: " << t << " is missing 'destination' state name!" << endl;
				return NULL;
			}

			if (!t.isMember("callback") || !t["callback"].isString()) {
				cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: " << t << " is missing 'callback' name!" << endl;
				return NULL;
			}
		}

		Json::Value timers;
		if (sm.isMember("timers")) {
			timers = sm["timers"];

			if (!timers.isArray()) {
				cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: StateMachine definition's 'timers' must be an array: " << timers.asString() << endl;
				return NULL;
			}
			for (Json::Value::iterator timer = timers.begin(); timer != timers.end(); timer++) {
				Json::Value &t = *timer;
				if (!t.isObject()) {
					cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: StateMachine's 'timers' array must contain timer transitions only: " << t.asString() << endl;
					return NULL;
				}

				if (!t.isMember("state") || !t["state"].isString()) {
					cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: " << t << " is missing initial state 'name'!" << endl;
					return NULL;
				}

				if (!t.isMember("transition") || !t["transition"].isString()) {
					cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: " << t << " is missing transition 'name'!" << endl;
					return NULL;
				}

				if (!t.isMember("timeout") || !t["timeout"].isInt64()) {
					cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: " << t << " is missing 'timeout' value!" << endl;
					return NULL;
				}
			}
		}

		// we've got everything ready, build the state machine

		// create the sm
		smP = new StateMachine(machine_name);

		// add the states
		for (Json::Value::iterator state = states.begin(); state !=  states.end(); state++)
			smP->AddState((*state).asString());

		// set the initial state
		smP->SetInitialState(initial_state);

		// add the transitions
		for (Json::Value::iterator transition = transitions.begin(); transition != transitions.end(); transition++) {
			Json::Value &t = *transition;

			State  *srcP = smP->GetState(t["source"].asString());
			State  *dstP = smP->GetState(t["destination"].asString());
			const string &transition_name = t["name"].asString();
			const string &callback_name = t["callback"].asString();
			if (srcP && dstP)
				srcP->SetTransition(dstP, transition_name, callback_name);
			else
				cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: " << t << " invalid source or destination state(s)!" << endl;
		}

		// add the timers
		if (!timers.empty()) {
			for (Json::Value::iterator timer = timers.begin(); timer != timers.end(); timer++) {
				Json::Value &t = *timer;

				State  *srcP = smP->GetState(t["state"].asString());
				if (srcP) {
					const string &transition_name = t["transition"].asString();
					unsigned long timeout = (unsigned long )t["timeout"].asInt64();
					srcP->SetTimeoutTransition(timeout, transition_name);
				} else
					cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: " << t << " invalid timeout source state!" << endl;
			}
		}
	} catch (const Json::RuntimeError &e) {
		cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error:" << endl << e.what() << endl;
		return NULL;
	}

	return smP;
}
