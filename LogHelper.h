/*
 * LogHelper.h
 */

#ifndef LOGHELPER_H_
#define LOGHELPER_H_

#include <cstdarg>
#include <iostream>
#include <string>
#include <ctime>
#include <vector>

#ifndef TARGET_FREERTOS
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif


class LogHelper {
public:
	LogHelper();
	LogHelper(LogHelper* producer, const std::string& producerId, const std::string& producerName);
	virtual ~LogHelper();

	void log(const unsigned short logLevel, const char* format, ...);
	virtual bool willLog(const std::string &producerId , const unsigned short logLevel);
	virtual timespec getTime();
	virtual void writeLog(unsigned short level, std::string& logData, std::string& producerId, std::string& producerName);

	enum Mode {
		COUT,
		UDP,
		UART
	};
	/**
	 * Níveis de filtro do log helper
	 */
	static const unsigned short	FILTER_CF_EVENT = 0;
	static const unsigned short	FILTER_ERROR = 1;
	static const unsigned short	FILTER_EVENT = 2;
	static const unsigned short	FILTER_DATAFLOW = 3;
	static const unsigned short	FILTER_INFO = 4;
	static const unsigned short	FILTER_DEBUG = 5;
	static const unsigned short	FILTER_THREAD = 6;
	static const unsigned short	FILTER_MUTEX = 7;

	/**
	 * Values ranging from 10 to 26 are reserved for 16 debugging levels.
	 */
	static const unsigned short LOG_LEVEL_OTHER		= 10;
	static const unsigned short LOG_LEVEL_INFO		= 11;
	static const unsigned short LOG_LEVEL_DATAFLOW	= 12;
	static const unsigned short LOG_LEVEL_EVENT		= 13;
	static const unsigned short LOG_LEVEL_ERROR		= 14;
	static const unsigned short LOG_LEVEL_POTHER	= 15;
	static const unsigned short LOG_LEVEL_PDEBUG	= 16;
	static const unsigned short LOG_LEVEL_PINFO		= 17;
	static const unsigned short LOG_LEVEL_PDATAFLOW	= 18;
	static const unsigned short LOG_LEVEL_PEVENT	= 19;
	static const unsigned short LOG_LEVEL_PERROR	= 20;
	static const unsigned short LOG_LEVEL_WAKEUP	= 21;
	static const unsigned short LOG_LEVEL_SLEEP		= 22;
	static const unsigned short LOG_LEVEL_DEBUG		= 23;
	static const unsigned short LOG_LEVEL_SHOW_A	= 24;
	static const unsigned short LOG_LEVEL_SHOW_B	= 25;
	static const unsigned short LOG_LEVEL_CFEVENT	= 26;

	virtual void setLogMode(Mode newLogMode);
	virtual void setUdpPort(int udpport);
	virtual void setUdpAddr(char* udpaddr);
	virtual void setFilterLogLevel(unsigned short filterLogLevel);
	virtual void setLogThreads(bool logThreads);

private:
	LogHelper* _producer;
	std::string _producerId;
	std::string _producerName;
	Mode _logMode;
	unsigned short _filterLogLevel;
	bool _logThreads;
	struct Level{
		int level;
		std::string producerId;
	};
	std::vector<Level> logLevels;

#ifdef TARGET_FREERTOS
	char buf[128];
#else
	char buf[1024];
#endif

	int _udpport;
	std::string _udpaddr;
	struct sockaddr_in _logaddr;
	int _logsockfd;
 };

#endif /* LOGHELPER_H_ */
