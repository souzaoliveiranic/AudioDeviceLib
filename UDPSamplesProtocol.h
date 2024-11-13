/*
 * UDPSamplesProtocol.h
 *
 *  Created on: 31 de out. de 2024
 *      Author: fmce520
 */

#ifndef IMBEL_UTIL_UDPSAMPLESPROTOCOL_H_
#define IMBEL_UTIL_UDPSAMPLESPROTOCOL_H_

#include <string>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <vector>

class UDPSamplesProtocol
{

public:

	UDPSamplesProtocol();
	virtual ~UDPSamplesProtocol();

	enum payload_type{
		REAL,
		COMPLEX,
		UNSIGNED_CHAR,
		SHORT
	};
	struct Header{
		unsigned int sequence_number = 0;
		unsigned int sample_rate;
		payload_type type;
		float info;
	};

	struct UDPPacket{
		unsigned long size;
		Header header;
		unsigned char* udpPayload;
		int portNumber;
		std::string destAddr;
	};

	struct PerformaceUDPPacket{
		unsigned long size;
		unsigned char* udpPayload;
		int portNumber;
	};

	virtual UDPPacket* mountUdpSamplesProtocol( Header header, unsigned char* samples, unsigned long size);
	virtual PerformaceUDPPacket* mountPerformanceProtocol(std::string name, int performanceNumber);
	virtual void retrieveUdpSamplesProtocol(UDPPacket* udpPacket, unsigned char* samples, unsigned long totalSize);

};


#endif /* IMBEL_UTIL_UDPSAMPLESPROTOCOL_H_ */
