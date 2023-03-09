#include <stdio.h>
#include <unistd.h>
#include <jsoncpp/json/json.h>

#include <iostream>
#include <fstream>

#include "StateMachine.h"

Thread		  	 		StateMachine::m_garbage_collector_thread(&GarbageCollectorCallback);
list<StateMachine *>	StateMachine::m_machines;

/**
 * \fn StateMachine::StateMachine(string &name, void *user_dataP)
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
 * \fn void StateMachine::Release()
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
 * \fn StateMachine::~StateMachine()
 * \brief StateMachine destructor, clear all of the State Machine content. DO NOT
 *        CALL DIRECTLY. Call Release instead. The Garbage collector will free this
 *        SM later on when its safe (no pending threads)
 *
 */
StateMachine::~StateMachine() {
#ifdef SM_TRACES
	LogVText(SM_MODULE, 0, true, "State Machine %p is being destroyed", this);
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
 * \fn void StateMachine::GarbageCollectorCallback(void *parametersP)
 * \brief StateMachine garbage collector destroys the exiting state machines
 * 		  every once in a while, ensuring all pending callback threads have
 * 		  triggered or have been cancelled before then.
 *
 */
void StateMachine::GarbageCollectorCallback(void *parametersP) {
	bool exit = false;

#ifdef SM_TRACES
	LogText(SM_MODULE, 0, true, "State Machine garbage collector is starting");
#endif

	while (!exit) {
		sleep(GARBAGE_COLLECTOR_PAUSE);

		// go through all the state machines and delete those that are declared as exiting
		list<StateMachine *>::iterator i = m_machines.begin();
		while (i != m_machines.end()) {
			StateMachine *smP = *i++;

			if (smP->IsExiting() && !smP->IsPendingThread()) {
#ifdef SM_TRACES
				LogVText(SM_MODULE, 4, true, "State Machine garbage collector is deleting sm %p", smP);
#endif
				m_machines.remove(smP);
				delete smP;
			}
		}

		if (m_machines.empty())
			exit = true;
	}

#ifdef SM_TRACES
	LogText(SM_MODULE, 0, true, "State Machine garbage collector is exiting");
#endif

	return;
}

/**
 * \fn void StateMachine::CallbackThread(void *parametersP)
 * \brief Creates and starts a new thread to call a transition callback
 *
 * \param parametersP points to a CallbackThreadParameters structure
 */
void StateMachine::CallbackThread(void *parametersP) {
	CallbackThreadParameters *paramsP = (CallbackThreadParameters *)parametersP;
	if (!paramsP || !paramsP->m_machineP || paramsP->m_machineP->IsExiting()) {
		paramsP->m_machineP->m_callback_threads.remove(paramsP->m_threadP);
		return;
	}

#ifdef SM_TRACES
	LogVText(SM_MODULE, 0, true, "Callback thread launching callback %s for transition %s", paramsP->m_callback_name.c_str(), paramsP->m_transition_name.c_str());
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
	return;
}

/**
 * \fn void *StateMachine::InitiateTransitionTimer(unsigned long milli_sec_timeout, string &transition_name)
 * \brief Creates and starts a timer which will inject a transition into the current state when
 *        the given time has elapsed.
 *
 * \param milli_sec_timeout is the delay before the transition is injected in the state machine
 * \param transition_name is the injected transition's identifier
 */
void StateMachine::TimerThreadCallback(void *parametersP) {
	TimerThreadParameters *paramsP = (TimerThreadParameters *)parametersP;

	if (!paramsP || !paramsP->m_machineP)
		return;

#ifdef SM_TRACES
	LogVText(SM_MODULE, 0, true, "timerThread sleeping for %ld milliseconds", paramsP->m_milli_seconds);
#endif

	// if 0 returned, the cancelled semaphore was inc'ed by the state machine
	if (paramsP->m_cancelled.WaitA(paramsP->m_milli_seconds)) {

		paramsP->m_machineP->m_timer_sem.A();

		if (!paramsP->m_machineP->m_timer_thread.IsAskedToStop()) {
			#ifdef SM_TRACES
			LogText(SM_MODULE, 0, true, "timeout!!!");
			#endif
			paramsP->m_machineP->m_timer_sem.R();
			paramsP->m_machineP->DoTransition(paramsP->m_transition_name, true);
		} else
			paramsP->m_machineP->m_timer_sem.R();
	}
#ifdef SM_TRACES
	else LogText(SM_MODULE, 0, true, "TimerThread cancelled !!!");
#endif

	return;
}

/**
 * \fn void StateMachine::InitiateTransitionTimer(unsigned long milli_sec_timeout, const string &transition_name)
 * \brief Clears the current transition timer (timeout) if any, then sets a new
 * 		 transition timer to the the given transition.
 *
 * \param milli_sec_timeout is the new timer duration
 * \param transition_name is the name of the target transition (upon timer tiggering)
 */
void StateMachine::InitiateTransitionTimer(unsigned long milli_sec_timeout, const string &transition_name) {
	// cancel pending timer if any
	CancelTransitionTimer();

#ifdef SM_TRACES
	LogVText(SM_MODULE, 0, true, "Initiating transition timer to %ld ms", milli_sec_timeout);
#endif

	m_timer_sem.A(); 
	m_timer_thread.Run(new TimerThreadParameters(this, milli_sec_timeout, transition_name));
	m_timer_sem.R();
}

/**
 * \fn StateMachine *StateMachine::CreateFromDefinition(string& json_defintion_filename)
 * \brief This factory method creates a state machine from the given definition.
 *
 * \param json_defintion_filename is the JSON SM definition file.
 *
 * \return the created machine or NULL if an error occured.
 */
StateMachine *StateMachine::CreateFromDefinition(string &json_definition_filename) {
#ifdef SM_TRACES
	LogVText(SM_MODULE, 0, true, "StateMachine::CreateFromDefinition(%s)", json_definition_filename.c_str());
#endif

	ifstream file(json_definition_filename);
	if (!file.is_open()) {
		cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error: couldn't open " << json_definition_filename << "!" << endl;
		return NULL;
	}

	// create the state machine from the file definition
	return CreateFromDefinition(file);
}

/**
 * \fn StateMachine *StateMachine::CreateFromDefinition(istream& json_stream)
 * \brief This factory method creates a state machine from the given definition.
 *
 * \param json_stream is the JSON SM definition input stream.
 *
 * \return the created machine or NULL if an error occured.
 */
StateMachine *StateMachine::CreateFromDefinition(istream &json_stream) {
	Json::Value 	root;   		// 'root' will contain the root value after parsing.
	StateMachine	*smP = NULL;

	try {
		json_stream >> root;

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

		// if a timer, set it
		State *initialStateP = (State *)smP->GetState();
		unsigned long timeOut = initialStateP->GetTimeout();
		if (timeOut != 0)
			smP->InitiateTransitionTimer(timeOut, initialStateP->GetTimeoutTransition());

	} catch (Json::RuntimeError &e) {
		cerr << __FILE__ << ", " << __FUNCTION__ << "(" << __LINE__ << ") Error:" << endl << e.what() << endl;
		return NULL;
	}

	return smP;
}
