///////////////////////////////////////////////////////////
//  AudioDeviceLib.h
//  Implementation of the Class AudioDeviceLib
//	Version: 1.4.1
///////////////////////////////////////////////////////////

#if !defined(AUDIODEVICELIB_H)
#define AUDIODEVICELIB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <sched.h>
#include <errno.h>
#include <getopt.h>
#include <alsa/asoundlib.h>
#include <sys/time.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <pthread.h>

#include "CircularBuffer.h"
#include "UDPSamplesReceiver.h"
#include "UDPSamplesSender.h"
#include "Timerfd.h"
#include "LogHelper.h"
#include "AudioFilter.h"
#include "InputEventMonitor.h"
#include "GpioMonitor.h"
#include "VoiceActivityDetection.h"

#define ALERTS_STREAM_ID 100 //FIXME: enum para isso
#define GPIO_PTT_OFFSET 100 //FIXME: melhorar essa logica
#define UDP_STREAM_ID 101

struct mixStruct{
	unsigned short streamId;
	std::queue<std::vector<signed short>> payloads;
};

class AudioDeviceLib;

class PttInputEventThread : public Runnable
{
private:
	InputEventMonitor inputEventMonitor;
public:
	PttInputEventThread(AudioDeviceLib *ad);

	AudioDeviceLib *audioDeviceLibInstance;
	bool started;
	size_t threadSize;
	int threadPriority;
	virtual void start(size_t threadSize, int threadPriority);
	virtual void run();
};

class PttGpioThread : public Runnable
{
private:
	GpioMonitor gpioMonitor;
public:
	PttGpioThread(AudioDeviceLib *ad);

	AudioDeviceLib *audioDeviceLibInstance;
	bool started;
	size_t threadSize;
	int threadPriority;
	virtual void start(size_t threadSize, int threadPriority);
	virtual void run();
};

class AlertsThread : public Runnable
{
public:
	AlertsThread(AudioDeviceLib *ad);

	AudioDeviceLib *audioDeviceLibInstance;
	bool started;

	pthread_cond_t finishCond;
	pthread_mutex_t finishLock;
	pthread_mutex_t playLock;
	pthread_cond_t playCond;

	unsigned short _durationInMs;
	unsigned short _frequencyInHz;

	virtual void startTone(unsigned short durationInMs, unsigned short frequencyInHz);
	virtual void start(size_t threadSize, int threadPriority);
	virtual void run();
};


class MicThread : public Runnable
{
public:
	MicThread(AudioDeviceLib *ad);

	AudioDeviceLib *audioDeviceLibInstance;
	AudioFilter audioFilter;
	bool started;
	virtual void start(size_t threadSize, int threadPriority);
	virtual void run();
};

class SpeakerThread : public Runnable
{
public:
	SpeakerThread(AudioDeviceLib *ad);

	AudioDeviceLib *audioDeviceLibInstance;
	AudioFilter audioFilter;
	bool started;
	virtual void start(size_t threadSize, int threadPriority);
	virtual void run();
};

class AudioUDPSamplesReceiver: public UDPSamplesReceiver {
public:
	AudioUDPSamplesReceiver(AudioDeviceLib *ad);
	AudioDeviceLib *audioDeviceLibInstance;
	/**
	 * @brief Funcao chamada quando um pacote de amostras for recebido por udp para o speaker
	 */
	virtual void receiveCallback(UDPPacket* udpPacket);
};

class AudioDeviceLib
{

public:
	//************ Atributos ************//

	/** Threads **/
	MicThread micThread;
	SpeakerThread speakerThread;
	PttInputEventThread pttInputEventThread;
	PttGpioThread pttGpioThread;
	AlertsThread alertsThread;

	/** VAD **/
	VoiceActivityDetection voiceActivityDetector;

	/** UDP Samples Protocol **/
	UDPSamplesSender* udpSamplesSender;
	AudioUDPSamplesReceiver udpSamplesReceiver;

	/** Mutex e Signals **/
	pthread_mutex_t pttLock;
	pthread_cond_t pttCond;

	pthread_mutex_t runSpeakerLock;
	pthread_cond_t runSpeakerCond;
	pthread_mutex_t runMicLock;
	pthread_cond_t runMicCond;

	pthread_mutex_t mixLock;
	pthread_mutex_t micFIFOLock;

	/** Log **/
	LogHelper* logHelperProducer;
	std::string identifier;

	/** Alsa Device Names **/
	std::string speakerDevice;
	std::string micDevice;

	/** Buffer de Teste **/
	std::vector<float> sineBuffer;

	/** Variaveis **/
	unsigned long internalPayloadSize;
	unsigned long desiredPayloadSize;
	unsigned long micBufferSizeInMs;

	float speakerVolume;
	float alertsVolume;
	float speakerVolumeInDb;
	unsigned short rate;
	int gpioPttPollingPeriodInMs;

	bool endOfStream;
	bool loopback;
	bool speakerStarted, micStarted;
	bool streamingStatus;
	bool virtualPTT;
	bool cleanBufferOnStart;
	bool useVoiceActivityDetection;

	bool udpSenderMic;
	bool udpSenderSpeaker;
	int udpMicPort, udpSpeakerPort;
	std::string udpDestAddr;
	bool udpReceiverSpeaker;
	int  udpReceiverSpeakerPort;

	bool sineDebug;
	bool runningStatus;
	bool ptt;
	int pttNumber;

	/** Buffers **/
	CircularBuffer<signed short> shortMicCircularBuffer;
	std::vector<mixStruct> mixStructSequence;

	/** PTT Input Events **/
	std::map<std::string, std::vector<int>> inputEventFileAndKeyCodeMap;

	/** PTT Gpios **/
	std::map<std::string, int> gpioChipAndLineMap;

	//************ Metodos ************//

	AudioDeviceLib();
	virtual ~AudioDeviceLib();

	/****** Metodos de Criacao/Start/Callback/Push ******/

	/**
	 * @brief sobrescreva o metodo abaixo para utilizar as amostras gravadas pelo microfone
	 * este metodo eh utilizado como callback assim que os pacotes sao gravados pelo microfone
	 */
	virtual void micStreamPushPacket(std::vector<signed short> &payload) = 0;

	/**
	 * @brief sobrescreva o metodo abaixo para utilizar o evento de pressinamento de algum ptt
	 * pttNumber baseado na lista de keys a ser monitorada
	 */
	virtual int setPTT(bool ptt, int pttNumber, int keyCode);

	/**
	 * @brief use estah funcao para reproduzir pacotes no speaker
	 */
	virtual void speakerStreamPushPacket(unsigned short streamId, std::vector<signed short> &payload);

	/**
	 * @brief sobrescreva o metodo abaixo para utilizar o evento voz detectada (bloqueante)
	 */
	virtual void voiceDetected(bool voiceDetected);

	/**
	 * @brief Metodo bloqueante para reproducao de alertas sonoros
	 */
	virtual void startTone(unsigned short durationInMs, unsigned short frequencyInHz);

	/**
	 * @brief Cria as threads utilizadas pelo componente
	 * @return -2: dispositivo nao configurado
	 * @return -1: erro na inicializacao do dispositivo
	 */
	int start(size_t micThreadSize, int micThreadPriority,
			size_t speakerThreadSize, int speakerThreadPriority,
			size_t pttThreadSize, int pttThreadPriority,
			size_t alertsThreadSize, int alertsThreadPriority);

	/**
	 * @brief libera a execucao das threads apos a sua criacao e inicializacao
	 */
	int running(bool newRunningProperty);

	/**
	 * @brief inicio envio de pacotes de audio a partir do buffer gravada pelo microfone
	 * utilizando indiretamente callback micStreamPushPacket
	 */
	int streaming(bool newStreaming);


	/****** Setters Obrigatorios ******/

	/**
	 * @brief Configura o dispositivo de reproducao (speaker)
	 */
	int setSpeakerDevice(std::string newSpeakerDevice);

	/**
	 * @brief Configura o dispositivo de gravacao (microfone)
	 */
	int setMicDevice(std::string newMicDevice);

	/**
	 * @brief Utilize estah funcao para configurar multiplos input events a serem monitorados como PTTs
	 */
	int configInputEventPTTs(std::map<std::string, std::vector<int>> newInputEventFileAndKeyCodeMap);

	/**
	 * @brief Utilize estah funcao para configurar multiplos gpios a serem monitorados como PTTs
	 */
	int configGpioPTTs(std::map<std::string, int> newGpioChipAndLineMap);


	/****** Debug UDP  ******/

	/**
	 * @brief Configura o envio de amostras via udp e a porta base e endereço de envio
	 */
	int configUdpSender(int micPort, int speakerPort, std::string addr);

	/**
	 * @brief Habilita o envio de amostras do microfone via udp
	 */
	int setUdpSenderMic(bool newUdpSenderMic);

	/**
	 * @brief Habilita o envio de amostras do speaker via udp
	 */
	int setUdpSenderSpeaker(bool newUdpSenderSpeaker);

	/**
	 * @brief Configura a recepção de amostras via udp para o speaker e a porta base
	 */
	int configUdpReceiver(int speakerPort);

	/**
	 * @brief Habilita o recebimento de amostras do speaker via udp
	 */
	int setUdpReceiverSpeaker(bool newUdpReceiverSpeaker);

	/****** Setters  ******/

	/**
	 * @brief Repassa um logHelper a ser utilizado. Caso não seja passado utiliza-ra o default
	 */
	int setLogHelper(LogHelper* newLogHelper);

	/**
	 * @brief Configura a taxa de audio. DEVE SER CHAMADO ANTES DE TODOS OS OUTROS SETTERS
	 */
	int setRate(unsigned short newRate);

	/**
	 * @brief Habilita o loopback de audio
	 */
	int setLoopback(bool newLoopback);

	/**
	 * @brief Habilita o ptt virtual
	 */
	int setVirtualSwitchPTT(bool newVirtualSwitchPTT);

	/**
	 * @brief Configura o tamanho de pacote desejado em ms
	 */
	int setDesiredPacketSizeInMs(unsigned long newDesiredPacketSizeInMs);

	/**
	 * @brief Configura o tamanho de buffer do microfone desejado em ms
	 */
	int setMicBufferSizeInMs(unsigned long newMicBufferSizeInMs);

	/**
	 * @brief Habilita o loopback de audio
	 */
	int setVoiceActivityDetection(bool newVoiceActivityDetection);

	/**
	 * @brief Habilita o debug de tom
	 */
	int setSineDebug(bool newSineDebug);

	/**
	 * @brief Habilita a limpeza do buffer no inicio do streaming
	 */
	int setCleanBufferOnStart(bool newCleanBufferOnStart);

	/**
	 * @brief Configura o ganho de volume do dispositivo em dB
	 */
	int setSpeakerVolume(float volumeInDb);


	//************ Metodos Internos ************//

	int micConfig();
	int micStart();
	int micStop();
	int speakerConfig();
	int speakerStart();
	int speakerStop();
	int pttConfig();

	long int readFromMic(void* samples, unsigned int samplesSize);
	long int writeToSpeaker(void* samples, unsigned int samplesSize);

private:
	snd_pcm_format_t micAudioFormat;    /* sample format */
	snd_pcm_format_t speakerAudioFormat;    /* sample format */

	snd_pcm_t *speakerHandle;
	snd_pcm_t *micHandle;

	int setHWParams(snd_pcm_t *cvad_handle, snd_pcm_hw_params_t *params,
			snd_pcm_access_t access, unsigned int channels, uint8_t ch_sel,
			unsigned int *buffer_time, unsigned int *period_time, unsigned int _rate,
			snd_pcm_sframes_t *buffer_size,
			snd_pcm_sframes_t *period_size,
			snd_pcm_format_t format);
	int setSWParams(snd_pcm_t *handle, snd_pcm_sw_params_t *swparams,
			snd_pcm_sframes_t *buffer_size,
			snd_pcm_sframes_t *period_size);
	int xrun_recovery(snd_pcm_t *handle, int err);



};
#endif // !defined(AUDIODEVICELIB_H)

