/*
 * UDPSamplesSender.h
 *
 *  Created on: 31 de out. de 2024
 *      Author: fmce520
 */

#ifndef IMBEL_UTIL_UDPSAMPLESENDER_H_
#define IMBEL_UTIL_UDPSAMPLESENDER_H_

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

class UDPSamplesSender;

class UDPSamplesSenderThread : public Runnable
{
public:
	UDPSamplesSenderThread(UDPSamplesSender *udpSamplesSender);

	UDPSamplesSender *udpSamplesSenderInstance;
	bool started;
	virtual void start(std::string name);
	virtual void run();
};

/**
 * @brief Classe singleton utilizada para enviar amostras via UDP no protocolo IMBEL
 *
 */

class UDPSamplesSender: public UDPSamplesProtocol
{
private:

	//Ponteiro estatico para a instancia do singleton
	static UDPSamplesSender* instancePtr;

	std::string name;
	std::string destAddr;

	//Thread
	UDPSamplesSenderThread udpSamplesSenderThread;

	UDPSamplesSender();
	virtual ~UDPSamplesSender();

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
	std::queue<UDPPacket*> udpPacketsQueue;

	bool started;


	//Deletando o construtor por copia para previnir copias
	UDPSamplesSender(const UDPSamplesSender& obj) = delete;
	void operator=(const UDPSamplesSender &) = delete;

	/**
	 * @brief Metodo estatico para uso da instancia do singleton
	 */
	static UDPSamplesSender* getInstance();

	/**
	 * @brief Configura o envio de pacotes;
	 */
	virtual int config(std::string name);

	/**
	 * @brief Envia o pacote de amostras via UDP para a porta e endereço ip especificado
	 * @param size eh tamanho do pacote de amostras em bytes
	 * @param header no header eh definido o tipo de dados que estah sendo enviado
	 */
	virtual int send(Header header, unsigned char* debugSamples, unsigned long size, int portNumber, std::string addr);

};


#endif /* IMBEL_UTIL_UDPSAMPLESENDER_H_ */
