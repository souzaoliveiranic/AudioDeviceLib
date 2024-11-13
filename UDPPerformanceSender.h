/*
 * UDPPerformanceSender.h
 *
 *  Created on: 31 de out. de 2024
 *      Author: fmce520
 */

#ifndef IMBEL_UTIL_UDPPERFORMANCESENDER_H_
#define IMBEL_UTIL_UDPPERFORMANCESENDER_H_

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

#ifndef TARGET_FREERTOS
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#endif
#ifdef TARGET_FREERTOS
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_IP.h"
#endif

#include <string.h>
#include <stdio.h>
#include <math.h>
#include <queue>

#include "Timerfd.h"
#include "Runnable.h"
#include "UDPSamplesProtocol.h"

class UDPPerformanceSender;

class UDPPerformanceSenderThread : public Runnable
{
public:
	UDPPerformanceSenderThread(UDPPerformanceSender *udpPerformanceSender);

	UDPPerformanceSender *udpPerformanceSenderInstance;
	bool started;
	virtual void start(char* name);
	virtual void run();
};

/**
 * @brief Classe singleton utilizada para enviar amostras via UDP no protocolo IMBEL
 *
 */

class UDPPerformanceSender: public UDPSamplesProtocol
{
private:

	//Ponteiro estatico para a instancia do singleton
	static UDPPerformanceSender* instancePtr;

	std::string name;
	int portBase;
	std::string destAddr;

	//Thread
	UDPPerformanceSenderThread udpPerformanceSenderThread;

	UDPPerformanceSender();
	virtual ~UDPPerformanceSender();

public:

	//Socket udp
#ifndef TARGET_FREERTOS
	struct sockaddr_in sockaddr;
	int sockfd;
#endif
#ifdef TARGET_FREERTOS
	struct freertos_sockaddr sockaddr;
	Socket_t sockfd;
#endif

	pthread_cond_t sendCond;
	pthread_mutex_t sendMutex;
	pthread_mutex_t queueMutex;

	//Fila de pacotes de amostras a ser enviado
	std::queue<PerformaceUDPPacket*> performanceUdpPacketsQueue;


	//Deletando o construtor por copia para previnir copias
	UDPPerformanceSender(const UDPPerformanceSender& obj) = delete;
	void operator=(const UDPPerformanceSender &) = delete;

	/**
	 * @brief Metodo estatico para uso da instancia do singleton
	 */
	static UDPPerformanceSender* getInstance();

	/**
	 * @brief Configura o endereço de destino do pacotes, o nome e a porta base;
	 */
	virtual int config(int portBase, char * addr, char* name);

	/**
	 * @brief Envia o pacote de performance via UDP para a porta base
	 *
	 */
	virtual int send(std::string name, int performanceNumber);

};


#endif /* IMBEL_UTIL_UDPPERFORMANCESENDER_H_ */
