/*
 * UDP_TRX.h
 *
 * Util class to send and receive Packages via UDP
 *
 *  Created on: 4 de ago. de 2021
 *      Author: fmce520
 */

#ifndef IMBEL_UTIL_UDP_TRX_H_
#define IMBEL_UTIL_UDP_TRX_H_

#if defined(TARGET_RONDON) || defined(TARGET_MALLET) || defined(TARGET_SIM)

#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <math.h>
#include <arpa/inet.h>
#include <queue>

#include "Timerfd.h"
#include "Runnable.h"
#include "UDPSamplesProtocol.h"
#include "CircularBuffer.h"


class UDP_TRX;

class WorkerRX : public Runnable
{
public:
	WorkerRX(UDP_TRX *udp_trx);

	UDP_TRX *udp_trxInstance;

	virtual void start();
	virtual void run();
};

class WorkerTX : public Runnable
{
public:
	WorkerTX(UDP_TRX *udp_trx);

	UDP_TRX *udp_trxInstance;

	virtual void start();
	virtual void run();
};

class UDP_TRX: public UDPSamplesProtocol
{
public:

	UDP_TRX();
	virtual ~UDP_TRX();

	WorkerTX workerTXRun;
	WorkerRX workerRXRun;

	pthread_cond_t rx_cond;
	pthread_mutex_t rx_mutex;
	pthread_mutex_t rx_queue_mutex;
	bool block_read;

	pthread_cond_t tx_cond;
	pthread_mutex_t tx_mutex;
	pthread_mutex_t tx_queue_mutex;

	std::queue<UDPPacket*> RXPacketsQueue;
	std::queue<UDPPacket*> TXPacketsQueue;
	CircularBuffer<unsigned char> RXPacketsFIFO;

	unsigned char* buffer_rx;
	unsigned int len;
	struct sockaddr_in servaddr_rx, cliaddr_rx;
	int sockfd_rx;
	int read_count;

	struct sockaddr_in sockaddr_tx;
	int sockfd_tx;


	virtual int createUDP( int txPort, int rxPort, char* addr);
	virtual int sendUDP(unsigned char* samples, unsigned long size);
	virtual int receiveUDP(unsigned char* samples, unsigned long size);
	int killReceiveUDP();
	void clearReceiveUDP();
};

#endif

#endif
