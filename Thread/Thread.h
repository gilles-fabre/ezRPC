#ifndef _THREAD_THREAD_H
#define _THREAD_THREAD_H

#include <thread>
#include <mutex>
#include <condition_variable>

#define THREAD_TRACES 1
#define THREAD_MODULE 0x1

typedef void ThreadCallbackFunction(void *);

using namespace std;

class Semaphore {
	mutex				m_mutex;
	condition_variable	m_cv;
	int					m_count;

public:
	Semaphore(int count) : m_count(count) {
	}

	// Acquire
	int Dec() {
		unique_lock<mutex> lock(m_mutex);
		while (m_count == 0) {
			m_cv.wait(lock);
		}
		--m_count;

		return 0; // could acquire sem.
	}
	int A() {
		return Dec();
	}
	int WaitA(unsigned long timeout) {
		unique_lock<mutex> lock(m_mutex);

		if (m_cv.wait_for(lock, timeout * 1ms, [this] {return m_count > 0; })) {
			--m_count;
			return 0;
		}

		return -1; // couldn't acquire sem
	}

	int TryA() {
		unique_lock<mutex> lock(m_mutex);

		if (m_count > 0) {
			--m_count;
			return 0;
		}

		return -1; // couldn't acquire sem
	}

	// Release
	int Inc() {
		unique_lock<mutex> lock(m_mutex);
		++m_count;
		m_cv.notify_all();

		return 0; // could release sem.
	}
	int R() {
		return Inc();
	}

	// value
	int Val() {
		unique_lock<mutex> lock(m_mutex);
		return m_count;
	}

	// reset to value
	void Reset(int value) {
		while (Val() > value)
			Dec();
		while (Val() < value)
			Inc();
	}
};

class Thread {
	ThreadCallbackFunction* m_callbackP;
	void*					m_parametersP;
	Semaphore				m_stopped;
	Semaphore				m_running;
	Semaphore				m_exited;
	thread*					m_threadP; 	  // to provide a timer based transition injection mechanism

	static void* IterateSafely(void* _threadP);
	static void* RunSafely(void* _threadP);

public:
	Thread(ThreadCallbackFunction* callbackP) : m_running(0), m_stopped(0), m_exited(0) {
		m_callbackP = callbackP;
		m_parametersP = NULL;
		m_threadP = NULL;
	}
	~Thread() {
		delete m_threadP;
	}

	void IterateAndWait(void *parametersP = NULL);
	void Iterate(void *parametersP = NULL);

	void RunAndWait(void *parametersP = NULL);
	void Run(void *parametersP = NULL);

	void Stop() {
		if (IsRunning())
			m_stopped.Inc();
	}

	void StopAndWait() {
		if (IsRunning()) {
			m_stopped.Inc();
			m_exited.A(); // blocks until thread actually exits
		}
	}

	bool IsRunning() {
		return m_running.Val() != 0;
	}

	bool IsAskedToStop() {
		return m_stopped.Val() != 0;
	}

	void *GetParameters() {
		return m_parametersP;
	}
};


#endif /* _THREAD_THREAD_H */
