/*
 * timerfd.h
 *
 *  Created on: 4 de ago. de 2021
 *      Author: fmce520
 *
 *
 */

#ifndef IMBEL_UTIL_TIMERFD_H_
#define IMBEL_UTIL_TIMERFD_H_

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
#else
#include <pthread.h>
#endif


#if defined(TARGET_RONDON) || defined(TARGET_MALLET) || defined(TARGET_SIM)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "unistd.h"
#include <sys/time.h>
#include <sys/timerfd.h>

struct periodic_info {
	int timer_fd;
	unsigned long long wakeups_missed;
};

#else

#include "task.h"

struct periodic_info {
	int timer_fd;
	unsigned long long wakeups_missed;
	unsigned long period; //ms
	TickType_t xLastWakeTime;
};

#endif

int make_periodic(unsigned long period, struct periodic_info *info);

/**
 * @brief Cria um timer com o period em Micro segundos
 */
int create_timer(unsigned long periodInuS, struct periodic_info *info);

/**
 * @brief Aguarda o tempo restante do periodo do timer
 */
int wait_period(struct periodic_info *info);



#endif /* IMBEL_UTIL_TIMERFD_H_ */

