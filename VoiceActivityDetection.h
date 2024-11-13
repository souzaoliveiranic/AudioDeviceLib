/*
 * VoiceActivityDetection.h
 *
 *  Created on: 21 de nov. de 2023
 *      Author: root
 */

#ifndef VOICE_ACTIVITY_DETECTION_H_
#define VOICE_ACTIVITY_DETECTION_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <math.h>
#include <vector>
#include <string>

class VoiceActivityDetection{
private:
	float RMS_min, SFM_min;
	uint32_t F_min;
	int speechCount, silenceCount, counter;
	float keepVADCount;
	bool lastVoiceDetected;
	unsigned short rate;

	int FreqThreshold   = 185;
	int SpeechThreshold = 5;
	int NoiseThreshold  = 10;
	int DelayThreshold  = 2;
	float SFMThreshold  = 5.0f;
	float RMSThreshold  = 40.0f;

	void ref_max_f32(float *psrc, uint32_t blockSize, float *pResult, uint32_t *pIndex);
	void ref_cmplx_mag_f32(std::vector<float>& audioSamplesFloat, float *pdst, uint32_t numSamples);
	void ref_cfft_f32 (uint32_t fftLen , std::vector<float>& audioSamplesFloat, uint8_t ifftFlag, uint8_t bitReverseFlag);
	bool voiceActivityDetectionCalc(std::vector<signed short>& audioSamplesFloat);

public:
	VoiceActivityDetection();
	virtual ~VoiceActivityDetection();

	void setThreshold(int FreqThreshold, int SpeechThreshold,
			int NoiseThreshold, int DelayThreshold,
			float SFMThreshold, float RMSThreshold);

	void setRate(unsigned short rate);

	bool detectVoice(std::vector<signed short>& audioSamples);
};



#endif /* VOICE_ACTIVITY_DETECTION_H_ */
