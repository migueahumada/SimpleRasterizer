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



class Audio
{
public:
  Audio() = default;
	Audio(const String& name, const String& filePath);
	~Audio();

	void load(const char* filename);

	XAUDIO2_BUFFER getAudioBuffer() const{
		return m_buffer;
	}

protected:
	friend class AudioAPI;
	friend class SoundEngine;

	WAVEFORMATEXTENSIBLE m_waveFile{ 0 };
	XAUDIO2_BUFFER m_buffer{ 0 };

	String m_name;
	String m_filePath;
	unsigned char* m_pDataBuffer = nullptr;
};

