#ifndef _THREAD_THREAD_H
#define _THREAD_THREAD_H

#include <pthread.h>
#include <signal.h>
#include <semaphore.h>

typedef void ThreadCallbackFunction(void *);

class Semaphore {
	sem_t m_semaphore;

public:
	Semaphore(int count) {
		sem_init(&m_semaphore, 0, count);
	}
	~Semaphore() {
		sem_destroy(&m_semaphore);
	}

	// Acquire
	int Dec() {
		return sem_wait(&m_semaphore);
	}
	int A() {
		return Dec();
	}
	int WaitA(unsigned long timeout) {
		unsigned long milliLeft = timeout % 1000;
		struct timespec delay = {__time_t(time(NULL) + (timeout / 1000)),
								 long(milliLeft * 1000000)};
		return sem_timedwait(&m_semaphore, &delay);
	}
	int TryA() {
		return sem_trywait(&m_semaphore);
	}

	// Release
	int Inc() {
		return sem_post(&m_semaphore);
	}
	int R() {
		return Inc();
	}

	// value
	int Val() {
		int value;
		if (!sem_getvalue(&m_semaphore, &value))
			return value;
		return 0;
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
	ThreadCallbackFunction *m_callbackP;
	void				   *m_parametersP;
	Semaphore			   m_stopped;
	Semaphore			   m_running;
	Semaphore			   m_exited;
	pthread_t 			   m_thread_id; 	  // to provide a timer based transition injection mechanism

	static void *IterateSafely(void *_threadP);
	static void *RunSafely(void *_threadP);

public:
	Thread(ThreadCallbackFunction *callbackP) : m_running(0), m_stopped(0), m_exited(0), m_thread_id(0) {
		m_callbackP = callbackP;
		m_parametersP = NULL;
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
			pthread_kill((unsigned long)m_thread_id, SIGINT); // in case the thread is stuck in the callback...
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
