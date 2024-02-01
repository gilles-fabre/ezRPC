#include "Thread.h"

#include <log2reporter.h>

void* Thread::IterateSafely(void* _threadP) {
	Thread *threadP = (Thread*)_threadP;
	if (!threadP || !threadP->m_callbackP)
		return NULL;

#ifdef THREAD_TRACES
	LogVText(THREAD_MODULE, 0, true, "Thread::IterateSafely(%p)", threadP);
#endif

	threadP->m_running.Reset(1);
	threadP->m_exited.Reset(0);
	threadP->m_stopped.Reset(0);

	while (threadP->m_stopped.WaitA(1)) {
#ifdef THREAD_TRACES
		LogVText(THREAD_MODULE, 4, true, "Thread iteration (%p)", threadP);
#endif
		(*(threadP->m_callbackP))(threadP->m_parametersP);
	}

	threadP->m_running.Dec();
	threadP->m_exited.Inc();

#ifdef THREAD_TRACES
	LogVText(THREAD_MODULE, 0, true, "Thread::IterateSafely(%p) exiting", threadP);
#endif

	return NULL;
}

void* Thread::RunSafely(void* _threadP) {
	Thread* threadP = (Thread*)_threadP;
	if (!threadP || !threadP->m_callbackP)
		return NULL;

#ifdef THREAD_TRACES
	LogVText(THREAD_MODULE, 0, true, "Thread::RunSafely(%p)", threadP);
#endif

	threadP->m_running.Reset(1);
	threadP->m_exited.Reset(0);
	threadP->m_stopped.Reset(0);

#ifdef THREAD_TRACES
	LogVText(THREAD_MODULE, 4, true, "Thread run (%p)", threadP);
#endif

	(*(threadP->m_callbackP))(threadP->m_parametersP);

	threadP->m_running.Dec();
	threadP->m_exited.Inc();

#ifdef THREAD_TRACES
	LogVText(THREAD_MODULE, 0, true, "Thread::RunSafely(%p) exiting", threadP);
#endif

	return NULL;
}

void Thread::IterateAndWait(void* parametersP) {
	if (IsRunning())
		return;

#ifdef THREAD_TRACES
	LogVText(THREAD_MODULE, 0, true, "Thread::IterateAndWait(%p)", parametersP);
#endif

	Iterate(parametersP);

	m_exited.A();

#ifdef THREAD_TRACES
	LogVText(THREAD_MODULE, 0, true, "Thread::IterateAndWait(%p) exiting", parametersP);
#endif
}

void Thread::Iterate(void* parametersP) {
	if (IsRunning())
		return;

#ifdef THREAD_TRACES
	LogVText(THREAD_MODULE, 0, true, "Thread::Iterate(%p)", parametersP);
#endif

	m_parametersP = parametersP;
	m_threadP = new thread(&IterateSafely, (void*)this);
	m_threadP->detach();

#ifdef THREAD_TRACES
	LogVText(THREAD_MODULE, 0, true, "Thread::Iterate(%p) thread spwaned", parametersP);
#endif
}

void Thread::RunAndWait(void* parametersP) {
#ifdef THREAD_TRACES
	LogVText(THREAD_MODULE, 0, true, "Thread::RunAndWait(%p) thread spwaned", parametersP);
#endif

	Run(parametersP);

	m_exited.A();

#ifdef THREAD_TRACES
	LogVText(THREAD_MODULE, 0, true, "Thread::RunAndWait(%p) exiting", parametersP);
#endif
}

void Thread::Run(void* parametersP) {
	if (IsRunning())
		return;

#ifdef THREAD_TRACES
	LogVText(THREAD_MODULE, 0, true, "Thread::Run(%p) thread spwaned", parametersP);
#endif

	m_parametersP = parametersP;
	m_threadP = new thread(&RunSafely, (void*)this);
	m_threadP->detach();

#ifdef THREAD_TRACES
	LogVText(THREAD_MODULE, 0, true, "Thread::Run(%p) thread spwaned", parametersP);
#endif
}

