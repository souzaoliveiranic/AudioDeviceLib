#ifndef RUNNABLE_HH
#define RUNNABLE_HH

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
#include "FreeRTOS_POSIX/sched.h"
}
#include "cmsis_gcc.h"
#else
#include <pthread.h>
#endif

// #include <pthread.h>
#include <unistd.h>
#include <map>
#include "Thread.h"

class Runnable
{
public:
	virtual ~Runnable();

	pthread_mutex_t  runningMutex;
	bool running;
	Thread * runThread;
	static std::map<pid_t,std::string> pidMap;
	static int memory_size;

	struct thread_parameters
	{
		size_t size;
		int priority;
		const char* name;
		int policy;
		//thread_parameters(): size(1024), priority(0), name("SCA"), policy(SCHED_OTHER){}
		thread_parameters( size_t s = 1024, int p = 0, const char* n = "SCA", int pol = SCHED_OTHER)
		{
			size = s;
			priority = p;
			name = n;
			policy = pol;
		}
	};

	thread_parameters threadparameters;

	virtual void run() = 0;
	virtual void startProcessing(thread_parameters param = thread_parameters());
	virtual void stopProcessing();
	virtual std::string printThreadInfo();
	virtual void printStackInfo();
	static unsigned long printStackInfo(unsigned long highWaterMark, std::string name);

	bool isRunning();
	virtual void setRunning(bool newIsRunningValue);
	virtual Thread* createThread();
	virtual pthread_t getThreadId();
	static std::string getNameByPid(pid_t pid);
	static std::string getNamesAndPids();

#ifdef TARGET_FREERTOS
	UBaseType_t highWaterMark;
#endif

protected:
	Runnable();
};

inline unsigned long Runnable::printStackInfo(unsigned long highWaterMark, std::string name)
{
#ifdef TARGET_FREERTOS
	std::string ret_msg;
    /* Inspect our own high water mark on entering the task. */
	UBaseType_t newHighWaterMark = uxTaskGetStackHighWaterMark( NULL );
	newHighWaterMark *= sizeof( StackType_t);
	if(newHighWaterMark < highWaterMark || highWaterMark == 0 || newHighWaterMark == 0){
		ret_msg = "---> Thread " + name;
		//stack size improved
		ret_msg += " unused " + std::to_string(newHighWaterMark*sizeof( StackType_t));
		std::cout << ret_msg << std::endl;
	}
	if(newHighWaterMark == 0){
		__BKPT();
	}
	return newHighWaterMark;
#else
	return 0;
#endif

}

#endif
