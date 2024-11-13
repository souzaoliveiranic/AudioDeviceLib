/*
 * GpioMonitor.h
 *
 *  Created on: 29 de out de 2024
 *      Author: fmce520
 */

#ifndef UTIL_GPIOMONITOR_H_
#define UTIL_GPIOMONITOR_H_

#ifndef TARGET_FREERTOS

#include <fcntl.h>
#include <gpiod.h>

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <map>
#include <vector>
#include <queue>

#include "Runnable.h"
#include "LogHelper.h"

class GpioMonitor;

class gpio_event{
public:
	bool value;
	int line;
};

//Thread que monitora input events
class GpioThread : public Runnable
{
private:
	struct gpiod_chip* _gpioChip;
	struct gpiod_line* _gpioLine;

	std::string gpioChipName;
	int gpioLine;
	int periodInMs;
public:
	GpioThread(GpioMonitor *iem);

	GpioMonitor *gpioMonitorInstance;
	bool started;

	/**
	 * @brief Configura o gpio e a linha a ser monitorada e o tempo do polling
	 *
	 * @return return -2 : nao eh possivel reconfigurar depois do dispositivo ter sido startado
	 * return -1 : arquivo nao pode ser aberto
	 */
	int config(std::string newGpioChip, int newLine, int newPeriodInMs);

	virtual void start(size_t threadSize, int threadPriority);
	virtual void run();
};

class GpioMonitor {
private:
	//threads para monitor cada input event file
	std::vector<GpioThread> gpioThreads;
	std::map<std::string, int> gpioChipAndLineMap;
	int periodInMs;

public:

	//log
	LogHelper* logHelperProducer;
	std::string identifier;

	//sinais para indicar a deteccao de um input event que estah sendo monitorado
	pthread_mutex_t signalLock;
	pthread_cond_t signalCond;

	//fila de eventos
	std::queue<struct gpio_event> events;

	GpioMonitor();
	virtual ~GpioMonitor();

	/**
	 * @brief Utilize estah funcao para configurar multiplos gpios a serem monitorados e periodInMso de monitoramento (polling).
	 * @param Mapa: gpio chip name, gpio line
	 * caso deseje monitor mais de uma linha na mesma gpio basta adicionar mais de uma entrada com a mesma gpio e linha diferentes
	 */
	int config(std::map<std::string, int> newGpioChipAndLineMap, int periodInMs);

	/**
	 * @brief inicia o monitoramento dos gpio events
	 */
	int start(size_t threadSize, int threadPriority);

	int setLogHelper(LogHelper* newLogHelper);

	/**
	 * @brief metodo para leitura bloqueante de todos os possiveis gpio configurados
	 */
	gpio_event readGpio();

};

#endif

#endif /* UTIL_GPIOMONITOR_H_ */
