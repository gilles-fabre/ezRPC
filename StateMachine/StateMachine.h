#ifndef STATEMACHINE_H_
#define STATEMACHINE_H_

#include <iostream>
#include <map>
#include <list>
#include <string>
#include <sstream>

#include <log2reporter.h>
#include <Thread.h>
#include <RPCClient.h>

using namespace std;

#define GARBAGE_COLLECTOR_PAUSE 5 // the garbage collector will work every <this amount of> seconds

#define SM_TRACES 1 // the state machine will printf traces
#define SM_MODULE 7

/**
 * \class Transition
 * \brief Holds a source State to destination State transition info. The source
 * 		  state is implicit (owns the transition), the callback can be NULL.
 */
class State;
class Transition {
	string	m_name;				// transition name
	State 	*m_stateP; 			// destination state
	string	m_callback_name;	// call this upon transition

public:
	Transition(const string &name, State *stateP, const string &callback_name) {
		m_name = name;							// transition name
		m_stateP = stateP;						// destination state
		m_callback_name = callback_name;		// state change callback
	}

	string &GetName() {
		return m_name;
	}
	State *GetState() {
		return m_stateP;
	}
	void SetState(State *stateP) {
		m_stateP = stateP;
	}

	const string &GetCallback() {
		return m_callback_name;
	}
	void SetCallback(const string &callback_name) {
		m_callback_name = callback_name;
	}
};

/**
 * \class State
 * \brief State represents a machine state, it owns the paths to the connected
 * 		  (through Events and Actions) States. The State is NOT thread safe.
 */
class State  {
	string 				 		m_name; 		// optional state name
	void						*m_user_dataP;	// user data pointer
	map<string, Transition *> 	m_transitions;	// linked states
	unsigned long				m_millisec_timeout;	  // if != 0, init transition timer when entering this
	string						m_timeout_transition; // invoke this one upon timeout

public:
	/**
	 * \fn State(const string &name, void *user_dataP)
	 * \brief State constructor
	 *
	 * \param name is the new state's name
	 * \param user_dataP can point to user data information block or be NULL
	 */
	State(const string &name, void *user_dataP) {
		m_millisec_timeout = 0;
		m_user_dataP = user_dataP;
		m_name = name;
	}
	/**
	 * \fn ~State()
	 * \brief State destructor. Clears the State and all its member data
	 *
	 */
	~State() {
		// delete transitions
		m_transitions.clear();
	}

	const string &GetName() {
		return m_name;
	}

	void *GetUserData() {
		return m_user_dataP;
	}

	unsigned long GetTimeout() {
		return m_millisec_timeout;
	}

	const string &GetTimeoutTransition() {
		return m_timeout_transition;
	}

	void SetTimeoutTransition(unsigned long millisec_timeout, const string &transition_name) {
		m_millisec_timeout = millisec_timeout;
		m_timeout_transition = transition_name;
	}

	/**
	 * \fn void SetTransition(State *destinationP, string &transition_name, const StateChangeCallback callback)
	 * \brief Creates a new or modifies an existing transition
	 *
	 * \param destinationP is the target State
	 * \param transition_name is the transition identifier
	 * \param callback_name will be called when the new transition is executed
	 */
	void SetTransition(State *destinationP, const string &transition_name, const string &callback_name) {
		Transition *transitionP = m_transitions[transition_name];
		if (transitionP) {
			// modify transition
			transitionP->SetState(destinationP);
			transitionP->SetCallback(callback_name);
		}
		else
			// insert new transition
			m_transitions[transition_name] = new Transition(transition_name, destinationP, callback_name);
	}

	/**
	 * \fn void SetTransitionCallback(const string &transition_name, const StateChangeCallback callback)
	 * \brief Modifies an existing transition's callback
	 *
	 * \param transition_ is the transition identifier
	 * \param callback will be called when the new transition is executed (reset to default if empty)
	 */
	void SetTransitionCallback(const string &transition_name, const string &callback_name) {
		Transition *transitionP = m_transitions[transition_name];
		if (transitionP)
			// modify transition callback
			transitionP->SetCallback(callback_name);
	}

	/**
	 * \fn void RemoveTransition(const string &transition_name, )
	 * \brief Deletes an existing transition
	 *
	 * \param name is the transition identifier
	 */
	void RemoveTransition(const string &transition_name) {
		Transition *transitionP = m_transitions[transition_name];
		if (transitionP) {
			m_transitions.erase(transition_name);
			delete transitionP;
		}
	}

	/**
	 * \fn State *DoTransition(const string &transition_name, StateChangeCallback *callbackP)
	 * \brief Executes the given transition on this, DOES NOT CALL
	 *        the associated callback.
	 *
	 * \param transition_name is the transition identifier
	 * \param *callbackP receives the callback (if not NULL)
	 *
	 * \return the newly reached State
	 */
	State *DoTransition(const string &transition_name, string &callback_name) {
		State *destinationP = this;

		// empty the callback in case we don't change state
		callback_name.clear();

		Transition *transitionP = m_transitions[transition_name];
		if (transitionP) {
			destinationP = transitionP->GetState();

			#ifdef SM_TRACES
			LogVText(SM_MODULE, 0, true, "State Machine execution transition %s from %s to %s", transitionP->GetName().c_str(), m_name.c_str(), destinationP->GetName().c_str());
			#endif

			// do we have a callback to invoke?
			callback_name = transitionP->GetCallback();
		}

		return destinationP;
	}

	/**
	 * \fn bool CheckTransition(const string &transition_name)
	 * \brief Checks whether the given transition exists for this
	 *
	 * \param name is the transition identifier
	 *
	 * \return true if and only if the transition exists
	 */
	bool CheckTransition(const string &transition_name) {

		Transition *transitionP = m_transitions[transition_name];

		if (!transitionP)
			cerr << "State Machine transition " << transition_name << " doesn't exist in state " << m_name << endl;

		return transitionP != NULL;
	}
};

/**
 * \class StateMachine
 * \brief Defines a dynamically programmable thread safe state machine. Only the
 * 		  DoTransition method is thread safe. The state machine can't be concurrently
 * 		  built/destroyed.
 */
class StateMachine {
	const RPCClient					*m_rpcClientP;		// to invoke the EClient callbacks
	string							m_name;	 	  		// in case we have different machines...
	void							*m_user_dataP; 	 	// user data
	map<string, State *> 			m_states; 		 	// all states
	Semaphore	  					m_call_sem;	 	 	// to protect against transitions/threads concurrency
	Thread 							m_timer_thread; 	// to provide a timer based transition injection mechanism
	Semaphore 						m_timer_sem; 	  	// do not transition if cancelled, do not cancel if transitionning
	static Thread 					m_garbage_collector_thread; // cleans up exiting machines when appropriate
	State 							*m_current_stateP; 	// this is where we stand
	Semaphore						m_exiting;			// this machine can be garbaged
	static list<StateMachine *> 	m_machines;			// all alive and exiting machines

	virtual ~StateMachine();

	static void GarbageCollectorCallback(void *parametersP);

public:
	StateMachine(string &name, void *user_dataP = NULL);
	void Release();

	const RPCClient *GetRpcClient() {
		return m_rpcClientP;
	}

	void SetRpcClient(const RPCClient *rpcClientP) {
		if (m_rpcClientP)
			delete m_rpcClientP;
			
		m_rpcClientP = rpcClientP;
	}

	/**
	 * \fn const string GetName()
	 * \brief returns the machine's name
	 *
	 */
	const string &GetName() {
		return m_name;
	}

	/**
	 * \fn int GetUserData()
	 * \brief returns the user data pointer
	 */
	void *GetUserData() {
		return m_user_dataP;
	}

	/**
	 * \fn int SetUserData(void *user_dataP)
	 * \brief sets the user data pointer
	 */
	void SetUserData(void *user_dataP) {
		m_user_dataP = user_dataP;
	}

	/**
	 * \fn bool SetInitialState(const string &state_name)
	 * \brief Sets this' state machine initial state. MUST be called
	 *        prior to injecting transitions on the state machine
	 *
	 * \param state_name is the initial state's name
	 *
	 * \return true if and only if the given state could be found
	 *
	 */
	bool SetInitialState(const string &state_name) {
		m_current_stateP = m_states[state_name];

		return m_current_stateP != NULL;
	}

	/**
	 * \fn const State *GetState()
	 *
	 * \return a pointer to the current state of the machine.
	 */
	const State *GetState() {
		return m_current_stateP;
	}

	/**
	 * \fn State *AddState(const string &state_name)
	 * \brief Adds a new state to this state machine
	 *
	 * \param state_name is the *unique* new state's name
	 *
	 * \return the new or existing (non unique name...) state
	 *
	 */
	State *AddState(const string &state_name) {
		State *stateP = m_states[state_name];

		// don't add state if already existing
		if (!stateP) {
			stateP = new State(state_name, m_user_dataP);
			m_states[state_name] = stateP;
		}

		return stateP;
	}

	State *GetState(const string &state_name) {
		return m_states[state_name];
	}

	/**
	 * \fn void SetTransition(State *sourcP, State *destinationP, const string &transition_name, const StateChangeCallback callback)
	 * \brief Creates a new or modifies an existing transition
	 *
	 * \param sourceP points to the source State
	 * \param destinationP points the target State
	 * \param transition_name is the transition's identifier
	 * \param callback will be called when the new transition is executed
	 */
	void SetTransition(State *sourceP, State *destinationP, const string &transition_name, const string &callback_name) {
		if (sourceP && destinationP)
			sourceP->SetTransition(destinationP, transition_name, callback_name);
	}

	/**
	 * \fn void SetTransitionCallback(State *sourceP, const string &transition_name, const string &callback_name)
	 * \brief Changes (or resets to default -a printf-) a transition's callback
	 *
	 * \param sourceP points to the transition's source state
	 * \param transition_name is the transition's identifier
	 * \param callback_name is the new transition's callback, can be NULL to default it
	 *
	 */
	void SetTransitionCallback(State *sourceP, const string &transition_name, const string &callback_name) {
		if (sourceP)
			sourceP->SetTransitionCallback(transition_name, callback_name);
	}

	void SetTransitionCallback(const string &state_name, const string &callback_name) {
		State *sourceP = GetState(state_name);
		if (sourceP)
			sourceP->SetTransitionCallback(state_name, callback_name);
	}

	/**
	 * \fn bool CheckTransition(const string &transtion_name)
	 * \brief Checks whether the given transition exists for the current state.
	 *
	 * \param transition_name is the transition's identifier
	 *
	 * \return true if the transition exists, false else.
	 */
	bool CheckTransition(const string &transition_name) {
		if (!m_current_stateP)
			return false;

		// check the transition
		m_call_sem.A();
		bool result = m_current_stateP->CheckTransition(transition_name);
		m_call_sem.R();

		return result;
	}

	/**
	 * \fn bool isPendingThread()
	 * \brief Checks whether the state machine has pending threads
	 *
	 * \return true if the state machine has threads still running
	 */
	inline bool IsPendingThread() {
		 bool pending;

		 //m_callSem.A(); // dead locks...
		 m_timer_sem.A();

		 pending = !m_callback_threads.empty() || m_timer_thread.IsRunning();

		 m_timer_sem.R();
		 //m_callSem.R();

		 return pending;
	}

	/**
	 * \fn void DoTransition(const string &transiPtion_name)
	 * \brief Executes the given transition on the current state.
	 *
	 * \param transition_name is the transition's identifier
	 * \param from_timer_thread shall be set when calling from the timer thread
	 *        to prevent the thread from being canceled prior to the transition.
	 */

	// callback thread parameters
	class CallbackThreadParameters {
	public:
		string		 m_callback_name;
		State		 *m_sourceP;
		State		 *m_destinationP;
		string		 m_transition_name;
		void		 *m_user_dataP;
		StateMachine *m_machineP;
		Thread		 *m_threadP;

		CallbackThreadParameters(Thread *threadP, StateMachine *machineP, const string &callback_name, State *sourceP, State *destinationP, const string &transition_name, void *user_dataP) {
			m_callback_name = callback_name;
			m_sourceP = sourceP;
			m_destinationP = destinationP;
			m_transition_name = transition_name;
			m_user_dataP = user_dataP;
			m_machineP = machineP;
			m_threadP = threadP;
		}
	};
	list<Thread *>  m_callback_threads;	 // all pending threads

	static void CallbackThread(void *parametersP);
	void DoTransition(const string &transition_name, bool from_timer_thread = false) {
		if (!m_current_stateP || IsExiting())
			return;

		// cancel pending timer if any
		if (!from_timer_thread)
			CancelTransitionTimer();

		// do inject the transition
		string callback_name;
		State *srcP;
		if ((srcP = m_current_stateP))
			m_current_stateP = m_current_stateP->DoTransition(transition_name, callback_name);

		// if a timer, set it
		if (m_current_stateP->GetTimeout() != 0)
			InitiateTransitionTimer(m_current_stateP->GetTimeout(), m_current_stateP->GetTimeoutTransition());

		// call into the callback if present
		// the state machine may be dying when the callback thread calls back into it
		// so, we need to keep a list of the threads, and wait for them when the SM dies
		// before actually destroying the SM (see garbage collector)
		Thread *threadP = new Thread(CallbackThread);

		// keep the ref of this thread in the SM's thread list
		m_call_sem.A();
		m_callback_threads.push_back(threadP);
		m_call_sem.R();

		// start the callback thread
		threadP->Run(new CallbackThreadParameters(threadP, this, callback_name, srcP, m_current_stateP, transition_name, m_user_dataP));
	}

	// timer triggered transition injection
	class TimerThreadParameters {
	public:
		StateMachine    *m_machineP;
		unsigned long	m_milli_seconds;
		string			m_transition_name;
		Semaphore		m_cancelled; // timer thread's cancellation semaphore (for a timed wait)

		TimerThreadParameters(StateMachine *machineP, unsigned long milli_seconds, const string &transition_name) : m_cancelled(0) {
			m_machineP = machineP;
			m_milli_seconds = milli_seconds;
			m_transition_name = transition_name;
		}

		TimerThreadParameters() : m_cancelled(0) {
			m_machineP = NULL;
			m_milli_seconds = 0;
		}

		TimerThreadParameters(const TimerThreadParameters &params) : m_cancelled(0) {
			m_machineP = params.m_machineP;
			m_milli_seconds = params.m_milli_seconds;
			m_transition_name = params.m_transition_name;
		}
	};

	static void TimerThreadCallback(void *parametersP);
	void 		InitiateTransitionTimer(unsigned long milli_sec_timeout, const string &transition_name);
	void 		CancelTransitionTimer() {
		m_timer_sem.A();

		if (m_timer_thread.IsRunning()) {
#ifdef SM_TRACES
			LogText(SM_MODULE, 0, true, "State Machine canceling transition timer");
#endif
			TimerThreadParameters *paramsP = (TimerThreadParameters *)m_timer_thread.GetParameters();
			if (paramsP)
				paramsP->m_cancelled.Inc();
			m_timer_thread.Stop(); 				// unlock the timer thread
		}

		m_timer_sem.R();
	}

	bool IsExiting() {
		return m_exiting.Val() != 0;
	}

	static StateMachine *CreateFromDefinition(string &json_definition_filename);
	static StateMachine *CreateFromDefinition(istream &json_definition_stream);
	
	static void printTransition(State *sourceP, State *destinationP, const string &transition_name, void *user_dataP);
};

#endif /* DAPSTATEMACHINE_H_ */
