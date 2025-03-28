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

class Audio
{
public:
  Audio() = default;
	Audio(const String& name, const String& filePath);
	~Audio() {
		if (m_pDataBuffer)
		{
			delete[] m_pDataBuffer;
			m_pDataBuffer = nullptr;
		}
	}

	void load(const char* filename);

protected:
	friend class AudioAPI;

	WAVEFORMATEXTENSIBLE m_waveFile{ 0 };
	XAUDIO2_BUFFER m_buffer{ 0 };

	String m_name;
	unsigned char* m_pDataBuffer = nullptr;
	IXAudio2SourceVoice* m_pSourceVoice = nullptr;
};

