#pragma once

#ifdef _XBOX //Big-Endian
#define fourccRIFF 'RIFF'
#define fourccDATA 'data'
#define fourccFMT 'fmt '
#define fourccWAVE 'WAVE'
#define fourccXWMA 'XWMA'
#define fourccDPDS 'dpds'
#endif

#ifndef _XBOX //Little-Endian
#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'
#endif

#include <xaudio2.h>
#include <fstream>
#include "HelperMacros.h"

using std::fstream;
using String = std::string;

class Submix;
class VoiceCallback;

class Audio
{
public:
  Audio() = default;
	Audio(const String& name, const String& filePath);
	~Audio();

	void load(const char* filename);

	void RouteTo(const WPtr<Submix>& submix, unsigned int flags = 0);

	IXAudio2SourceVoice* getSourceVoice() const{
		return m_pSourceVoice;
	}

protected:
	friend class AudioAPI;

	WAVEFORMATEXTENSIBLE m_waveFile{ 0 };
	XAUDIO2_BUFFER m_buffer{ 0 };

	String m_name;
	unsigned char* m_pDataBuffer = nullptr;
	IXAudio2SourceVoice* m_pSourceVoice = nullptr;



	//Sends
	XAUDIO2_VOICE_SENDS m_sends{0};
	Vector<XAUDIO2_SEND_DESCRIPTOR> m_sendList{0};

	//Effects
	XAUDIO2_EFFECT_CHAIN m_effects{0};
	Vector<XAUDIO2_EFFECT_DESCRIPTOR> m_effectList{0};
};

