///////////////////////////////////////////////////////////
//  Thread.h
//  Implementation of the Class Thread
//  Created on:      19-ago-2020 12:27:29
//  Original author: fmce520
///////////////////////////////////////////////////////////

#ifndef THREAD_HH
#define THREAD_HH


#include <iostream>
#include <cstdlib>

#ifdef TARGET_FREERTOS
//TODO Substituindo pthread.h pelo plugin POSIX + FreeRTOS
extern "C" {
#include "FreeRTOS_POSIX.h"
/* FreeRTOS+POSIX. */
#include "FreeRTOS_POSIX/pthread.h"
#include "FreeRTOS_POSIX/mqueue.h"
#include "FreeRTOS_POSIX/time.h"
#include "FreeRTOS_POSIX/fcntl.h"
#include "FreeRTOS_POSIX/unistd.h"
}
#else
#include <pthread.h>
#include <sys/syscall.h>

#ifndef TARGET_SIM
#ifndef SYS_gettid
#error "SYS_gettid unavailable on this system"
#endif
#define gettid() ((pid_t)syscall(SYS_gettid))
#endif
#endif

#include <unistd.h>
#include <limits.h>

//#define DEFAULT_STACK_SIZE    (512 * 1024)
//#define PTHREAD_STACK_MIN     4096
class Runnable;

class Thread
{

public:
	Thread(Runnable* target = 0);

	virtual ~Thread();

	void getName(char *name) const;
	void setName(const char* name);

	static void sleep(long millis, int nanos=0);
	int start();
	int join(void** retVal = 0);
	static void yield();

	int setStackSize(size_t size);
	size_t getStackSize();
	size_t getMinimumStackSize();

	int  getMaximumPriority() const;
	int  getMinimumPriority() const;
	int  getPriority() const;
	sched_param setPriority(int newPriority, int policy);
	int setParameters(sched_param sched_param);

	int interruptThread(int sig);
	virtual pthread_t getThreadId();
	virtual pid_t getThreadPid();
	virtual pid_t getThreadTid();

	static int createThread( pthread_t * thread,
			pthread_attr_t * attr,
			int priority,
			int policy,
			size_t stackSize,
			const char* name,
			void *( *startroutine )( void * ),
			void * arg );

	// virtual void startProcessing();
	// virtual void stopProcessing();
	// virtual void setRunning(bool newIsRunningValue);
	// virtual Thread* createThread();

protected:
	void setThreadReturnValue(void* retVal);

private:
	void init();
	static void* runAndWait(void *);

	// To avoid automatic copies.
	//Copy Constructor:
	Thread(const Thread& original);

	//Assignment :
	Thread& operator=(const Thread& original);

private:
	pthread_t thread;
	pthread_attr_t threadAttr;
	pthread_mutex_t * threadMutex;

	Runnable* target;
	//bool _isRunning;

	int   defaultRetVal;
	void* threadRetVal;
	int policy;
};

#endif //TREAD_HH
