/*
 * InputEventMonitor.h
 *
 *  Created on: 29 de out de 2024
 *      Author: fmce520
 */

#ifndef UTIL_INPUTEVENTMONITOR_H_
#define UTIL_INPUTEVENTMONITOR_H_

#ifndef TARGET_FREERTOS

#include <linux/input.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <map>
#include <vector>
#include <queue>

#include "Runnable.h"
#include "LogHelper.h"

class InputEventMonitor;

//Thread que monitora input events
class InputEventThread : public Runnable
{
private:
	std::ifstream file;
	std::string inputEventFile;
	std::vector<int> keyCodes;
public:
	InputEventThread(InputEventMonitor *iem);

	InputEventMonitor *inputEventMonitorInstance;
	bool started;

	/**
	 * @brief Configura o input event file e as key codes a serem monitoradas
	 *
	 * @return return -2 : nao eh possivel reconfigurar depois do dispositivo ter sido startado
	 * return -1 : arquivo nao pode ser aberto
	 */
	int config(std::string newInputEventFile, std::vector<int> newKeyCodes);

	virtual void start(size_t threadSize, int threadPriority);
	virtual void run();
};

class InputEventMonitor {
private:
	//threads para monitor cada input event file
	std::vector<InputEventThread> inputEventThreads;
	std::map<std::string, std::vector<int>> inputEventFileAndKeyCodeMap;

public:

	//log
	LogHelper* logHelperProducer;
	std::string identifier;

	//sinais para indicar a deteccao de um input event que estah sendo monitorado
	pthread_mutex_t signalLock;
	pthread_cond_t signalCond;

	//fila de eventos
	std::queue<struct input_event> events;

	InputEventMonitor();
	virtual ~InputEventMonitor();

	/**
	 * @brief Utilize estah funcao para configurar multiplos input events a serem monitorados.
	 * @param Mapa: "input envent file", key codes
	 * caso deseje monitor mais de uma key no mesmo arquivo basta adicionar mais de uma key no mesmo file
	 */
	int config(std::map<std::string, std::vector<int>>  newInputEventFileAndKeyCodeMap);

	/**
	 * @brief inicia o monitoramento dos input events
	 */
	int start(size_t threadSize, int threadPriority);

	int setLogHelper(LogHelper* newLogHelper);

	/**
	 * @brief metodo para leitura bloqueante de todos os possiveis input events configurados
	 */
	input_event readInputEvent();

};

#endif

#endif /* UTIL_INPUTEVENTMONITOR_H_ */
