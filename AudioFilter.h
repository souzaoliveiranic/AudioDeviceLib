/*
 * AudioFilter.h
 *
 *  Created on: 25 de nov de 2021
 *      Author: fmce520
 */

#ifndef IMBEL_UTIL_AudioFilter_H_
#define IMBEL_UTIL_AudioFilter_H_

#if defined(TARGET_RONDON) || defined(TARGET_SIM)

#include <liquid/liquid.h>

class AudioFilter {
private:
	unsigned long int audioRate;
	iirfilt_rrrf audioFilter;
public:
	AudioFilter();
	virtual ~AudioFilter();

	void createFilter(long int audioRate);
	void filterAudioSample(float sample, float* AudioFiltered_sample);
};
#endif

#endif /* IMBEL_UTIL_AudioFilter_H_ */
