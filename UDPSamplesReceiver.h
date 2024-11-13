/*
 * UDPSamplesReceiver.h
 *
 *  Created on: 31 de out. de 2024
 *      Author: fmce520
 */

#ifndef IMBEL_UTIL_UDPSAMPLESRECEIVER_H_
#define IMBEL_UTIL_UDPSAMPLESRECEIVER_H_

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

class UDPSamplesReceiver;

class UDPSamplesReceiverThread : public Runnable
{
public:
	UDPSamplesReceiverThread(UDPSamplesReceiver *udpSamplesReceiver);

	UDPSamplesReceiver *udpSamplesReceiverInstance;
	bool started;
	virtual void start(std::string name);
	virtual void run();
};


class UDPSamplesReceiver: public UDPSamplesProtocol
{
private:

	std::string name;
	int portBase;

	//Thread
	UDPSamplesReceiverThread udpSamplesReceiverThread;

public:

	//Socket udp
#ifndef TARGET_FREERTOS
	struct sockaddr_in servaddr, cliaddr;
	int sockfd;
#endif
#ifdef TARGET_FREERTOS
	//TODO: implementar para o tpp
#endif

	pthread_cond_t receiveCond;
	pthread_mutex_t receiveMutex;

	UDPSamplesReceiver();
	virtual ~UDPSamplesReceiver();

	/**
	 * @brief Configura o endereço de destino do pacotes, o nome e a porta base;
	 */
	virtual int config(int portBase, std::string name);

	/**
	 * @brief Caso se deseje usar leitura dos pacotes udp de forma assincrona (callback)
	 * eh necessario chamar a funcao abaixo para que a thread auxiliar seja criada
	 */
	virtual int startUDPSamplesReceiverThread(std::string name);

	/**
	 * @brief Recebe o pacote de amostras via UDP
	 * realizando uma leitura bloqueante
	 *
	 */
	virtual void receive(UDPPacket* udpPacket);

	/**
	 * @brief Sobrescreva o metodo abaixo para
	 * receber o pacote de amostras via UDP
	 * realizando uma leitura assincrona (callback).
	 * Sera criada uma therad e quando um pacote
	 * for recebido essa funcao será chamada
	 *
	 */
	virtual void receiveCallback(UDPPacket* udpPacket) = 0;

};


#endif /* IMBEL_UTIL_UDPSAMPLESRECEIVER_H_ */
