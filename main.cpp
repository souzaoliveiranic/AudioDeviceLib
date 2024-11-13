/**
 * main.cpp
 *
 * O objetivo deste exemplo eh testar a utilizacao da biblioteca AudioDeviceLib
 * Sao criados 3 Audio devices que serao mixados entre si, ou seja, ao apertar o ptt de um Audio Device seu audio capturado
 * pelo microfone eh enviado para ser reproduzido no speker dos outros.
 * Especialmente o primeiro canal tem um loopback habilitado e botoes de ptt para simular a mudanca de volume digital do canal.
 *
 *
 */

#ifdef EXECUTABLE_TEST

#include "AudioDeviceLib.h"

const int DEFAULT_BASE_PORT = 5600;

class AudioDeviceIntercom: public AudioDeviceLib {
public:
	int myID;
	//manipulando outros audios devices (mra)
	std::vector<AudioDeviceLib*> audioDevices;
	void setAudioDevices(AudioDeviceLib* ad){
		audioDevices.push_back(ad);
	}

	virtual void micStreamPushPacket(std::vector<signed short> &payload){
		for(unsigned int i = 0; i < audioDevices.size(); i++){
			audioDevices[i]->speakerStreamPushPacket(myID, payload);
		}
		//caso ptt 1: modo intercom se ouve tbm, caso contrario só os outros ouvem
		if(this->pttNumber == 1){
			speakerStreamPushPacket(0, payload);
		}
	};

	//sobrescrevendo o comportamento quando algum ptt eh detectado
	virtual int setPTT(bool ptt, int pttNumber, int keyCode){

		if(pttNumber == 0){
			//teste de ptt padrao
			if((!this->ptt && ptt) || (this->ptt && !ptt)){
				startTone(100, 800);
			}
			return AudioDeviceLib::setPTT(ptt, pttNumber, keyCode);
		} else if(pttNumber == 1){
			//teste de intercom, ativado pelo botao (toggle)
			if(ptt){
				return AudioDeviceLib::setPTT(!this->ptt, pttNumber, keyCode);
			}
		} else if(pttNumber == 2){
			//teste de aumento de volume
			if(ptt){
				setSpeakerVolume(speakerVolumeInDb + 3); //6db
			}
			return 0;
		} else if(pttNumber == 3){
			//teste de abaixar volume
			if(ptt){
				setSpeakerVolume(speakerVolumeInDb - 3);
			}
			return 0;
		}
		return 0;

	};

	void voiceDetected(bool voiceDetected){
		LogHelper logHelper(logHelperProducer, identifier, "AudioDeviceIntercom::voiceDetected");
		if(voiceDetected)
			logHelper.log(LogHelper::LOG_LEVEL_PEVENT, "Voice Detected");
		//		if(useVoiceActivityDetection)
		//			AudioDeviceLib::setPTT(voiceDetected, 0, 0);
	}


};


/**
 * Este exemplo testa o modo intercom do audio device.
 * Ou seja, diversas instancias de audio devices podem se comunicar
 */
void Exemplo1(){
	AudioDeviceIntercom audioDevice1;
	AudioDeviceIntercom audioDevice2;
	AudioDeviceIntercom audioDevice3;

	LogHelper logHelper;
	logHelper.setFilterLogLevel(LogHelper::FILTER_INFO);

	audioDevice1.identifier = "AudioDeviceIntercom 1";
	audioDevice2.identifier = "AudioDeviceIntercom 2";
	audioDevice3.identifier = "AudioDeviceIntercom 3";

	audioDevice1.myID = 1;
	audioDevice2.myID = 2;
	audioDevice3.myID = 3;

	audioDevice1.setLogHelper(&logHelper);
	audioDevice2.setLogHelper(&logHelper);
	audioDevice3.setLogHelper(&logHelper);

	audioDevice1.setSpeakerDevice("plughw:0,0");
	audioDevice1.setMicDevice("plughw:2,0");

	audioDevice2.setSpeakerDevice("plughw:1,0");
	audioDevice2.setMicDevice("plughw:1,0");

	//	audioDevice3.setSpeakerDevice("plughw:2,0");
	//	audioDevice3.setMicDevice("plughw:2,0");

	audioDevice2.setVoiceActivityDetection(true);

	audioDevice1.setAudioDevices(dynamic_cast<AudioDeviceLib*>(&audioDevice2));
	//	audioDevice1.setAudioDevices(dynamic_cast<AudioDeviceLib*>(&audioDevice3));
	audioDevice2.setAudioDevices(dynamic_cast<AudioDeviceLib*>(&audioDevice1));
	//	audioDevice2.setAudioDevices(dynamic_cast<AudioDeviceLib*>(&audioDevice3));
	audioDevice3.setAudioDevices(dynamic_cast<AudioDeviceLib*>(&audioDevice1));
	audioDevice3.setAudioDevices(dynamic_cast<AudioDeviceLib*>(&audioDevice2));

	std::map<std::string, std::vector<int>> newInputEventFileAndKeyCodeMap;

	newInputEventFileAndKeyCodeMap["/dev/input/event2"] = {KEY_1, KEY_2, KEY_UP, KEY_DOWN};
	newInputEventFileAndKeyCodeMap["/dev/input/event4"] = {BTN_FORWARD};
	audioDevice1.configInputEventPTTs(newInputEventFileAndKeyCodeMap);
	newInputEventFileAndKeyCodeMap.clear();

	newInputEventFileAndKeyCodeMap["/dev/input/event2"] = {KEY_3, KEY_4, KEY_UP, KEY_DOWN};
	audioDevice2.configInputEventPTTs(newInputEventFileAndKeyCodeMap);
	newInputEventFileAndKeyCodeMap.clear();

	newInputEventFileAndKeyCodeMap["/dev/input/event2"] = {KEY_5, KEY_2, KEY_UP, KEY_DOWN};
	audioDevice3.configInputEventPTTs(newInputEventFileAndKeyCodeMap);
	newInputEventFileAndKeyCodeMap.clear();



	audioDevice1.setUdpSenderMic(true);
	audioDevice1.setUdpSenderSpeaker(true);
	audioDevice1.configUdpSender(DEFAULT_BASE_PORT+1, DEFAULT_BASE_PORT+2,"255.255.255.255");

	audioDevice2.setUdpSenderMic(true);
	audioDevice2.setUdpSenderSpeaker(true);
	audioDevice2.configUdpSender(DEFAULT_BASE_PORT+3, DEFAULT_BASE_PORT+4,"255.255.255.255");

	audioDevice3.setUdpSenderMic(true);
	audioDevice3.setUdpSenderSpeaker(true);
	audioDevice3.configUdpSender(DEFAULT_BASE_PORT+5, DEFAULT_BASE_PORT+6,"255.255.255.255");

	audioDevice1.start(1024,10,1024,11,1024,12, 1024, 11);
	audioDevice1.running(true);
	audioDevice2.start(1024,10,1024,11,1024,12, 1024, 11);
	audioDevice2.running(true);
	//	audioDevice3.start(1024,10,1024,11,1024,12, 1024, 11);
	//	audioDevice3.running(true);

	while(1){
		sleep(1);
	}

}

class AudioDeviceUdp: public AudioDeviceLib {
public:

	virtual void micStreamPushPacket(std::vector<signed short> &payload){
		//Loopback para que o usuário saiba que o ptt foi pressionado
		speakerStreamPushPacket(0, payload);
	};

	//sobrescrevendo o comportamento quando algum ptt eh detectado
	virtual int setPTT(bool ptt, int pttNumber, int keyCode){
		//habilitando o envio via udp quando o ptt for pressionado
		if(this->ptt == false && ptt == true)
			setUdpSenderMic(true);
		else if(this->ptt == true && ptt == false)
			setUdpSenderMic(false);
		return AudioDeviceLib::setPTT(ptt, pttNumber, keyCode);
	}


	void voiceDetected(bool voiceDetected){
		LogHelper logHelper(logHelperProducer, identifier, "AudioDeviceIntercom::voiceDetected");
		if(voiceDetected)
			logHelper.log(LogHelper::LOG_LEVEL_PEVENT, "Voice Detected");
		//		if(useVoiceActivityDetection)
		//			AudioDeviceLib::setPTT(voiceDetected, 0, 0);
	}


};

void Exemplo2(std::string micDevice, std::string speakerDevice, int keyCode, int senderMicPort, int senderSpeakerPort, std::string senderAddr, int receiverSpeakerPort){
	AudioDeviceUdp audioDevice1;

	LogHelper logHelper;
	logHelper.setFilterLogLevel(LogHelper::FILTER_INFO);

	audioDevice1.identifier = "AudioDeviceUdp 1";

	audioDevice1.setSpeakerDevice(speakerDevice);
	audioDevice1.setMicDevice(micDevice);

//	audioDevice1.setVoiceActivityDetection(true);

	std::map<std::string, std::vector<int>> newInputEventFileAndKeyCodeMap;

	newInputEventFileAndKeyCodeMap["/dev/input/event2"] = {keyCode}; //{KEY_1, KEY_2, KEY_UP, KEY_DOWN};
	audioDevice1.configInputEventPTTs(newInputEventFileAndKeyCodeMap);
	newInputEventFileAndKeyCodeMap.clear();

	audioDevice1.configUdpSender(senderMicPort, senderSpeakerPort, senderAddr);
	audioDevice1.setUdpSenderSpeaker(true);
	audioDevice1.setUdpSenderMic(false);

	audioDevice1.configUdpReceiver(receiverSpeakerPort);
	audioDevice1.setUdpReceiverSpeaker(true);

	audioDevice1.start(1024,10,1024,11,1024,12, 1024, 11);
	audioDevice1.running(true);

	while(1){
		sleep(1);
	}
}


int __attribute__((weak)) main( int argc, char *argv[ ] ){

	int option;

	int senderMicPort = DEFAULT_BASE_PORT+1;
	int senderSpeakerPort = DEFAULT_BASE_PORT+2;
	std::string senderAddr = "255.255.255.255";
	int receiverSpeakerPort = DEFAULT_BASE_PORT+3;
	std::string micDevice = "plughw:0,0", speakerDevice = "plughw:0,0";
	int keyCode = KEY_1;

	/* Check command line options */
	while((option = getopt(argc, argv, "1:2:m:s:r:k:a:h")) > 0) {
		switch(option) {
		case '1':
			micDevice = optarg;
			break;
		case '2':
			speakerDevice = optarg;
			break;
		case 'm':
			senderMicPort = atoi(optarg);
			break;
		case 's':
			senderSpeakerPort = atoi(optarg);
			break;
		case 'a':
			senderAddr = optarg;
			break;
		case 'r':
			receiverSpeakerPort = atoi(optarg);
			break;
		case 'k':
			keyCode = atoi(optarg);
			break;
		case 'h':
			std::cout << "Usage:" << std::endl;

			std::cout << "\t-1  <micDevice>: Dispositivo alsa do microfone (arecord -l)" << std::endl;
			std::cout << "\t-2  <speakerDevice>: Dispositivo alsa do speaker (aplay -l)" << std::endl;
			std::cout << "\t-m  <senderMicPort>: Porta de envio das amostras do microfone" << std::endl;
			std::cout << "\t-s  <senderSpeakerPort>: Porta de envio das amostras do speaker" << std::endl;
			std::cout << "\t-a  <senderAddr>: Endereço de envio das amostras do microfone e do speaker" << std::endl;
			std::cout << "\t-r  <receiverSpeakerPort>: Porta de recepção da amostras de audio" << std::endl;
			std::cout << "\t-h: prints this help text" << std::endl;
			exit(1);
			break;
		default:
			break;
		}
	}

	Exemplo2(micDevice, speakerDevice, keyCode, senderMicPort, senderSpeakerPort, senderAddr, receiverSpeakerPort);

}

#endif
