#include "Thread.h"

void *Thread::IterateSafely(void *_threadP) {
	Thread *threadP = (Thread *)_threadP;
	if (!threadP || !threadP->m_callbackP)
		return NULL;

	threadP->m_running.Reset(1);
	threadP->m_exited.Reset(0);
	threadP->m_stopped.Reset(0);

	while (threadP->m_stopped.WaitA(1))
		(*(threadP->m_callbackP))(threadP->m_parametersP);

	threadP->m_running.Dec();
	threadP->m_exited.Inc();

	return NULL;
}

void *Thread::RunSafely(void *_threadP) {
	Thread *threadP = (Thread *)_threadP;
	if (!threadP || !threadP->m_callbackP)
		return NULL;

	threadP->m_running.Reset(1);
	threadP->m_exited.Reset(0);
	threadP->m_stopped.Reset(0);

	(*(threadP->m_callbackP))(threadP->m_parametersP);

	threadP->m_running.Dec();
	threadP->m_exited.Inc();

	return NULL;
}

void Thread::IterateAndWait(void *parametersP) {
	if (IsRunning())
		return;

	Iterate(parametersP);

	m_exited.A();
}

void Thread::Iterate(void *parametersP) {
	if (IsRunning())
		return;

	m_parametersP = parametersP;
	if (!pthread_create(&m_thread_id, NULL, &IterateSafely, (void *)this))
		pthread_detach(m_thread_id);
}

void Thread::RunAndWait(void *parametersP) {
	Run(parametersP);

	m_exited.A();
}

void Thread::Run(void *parametersP) {
	if (IsRunning())
		return;

	m_parametersP = parametersP;
	if (!pthread_create(&m_thread_id, NULL, &RunSafely, (void *)this))
		pthread_detach(m_thread_id);
}

