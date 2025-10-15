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

namespace BIT_DEPTH
{
	enum E
	{
		B8,
		B16,
		B24,
		B32
	};
}



class Audio
{
public:
  Audio() = default;
	Audio(const String& name, const String& filePath);
	~Audio();

	void load(const char* filename);

	const XAUDIO2_BUFFER& getAudioBuffer() const{
		return m_buffer;
	}

	//Total number of samples
	inline const size_t getNumSamples() const
	{
		return m_dataBufferSize / (m_bitDepth / 8);
	}

	//Number of channels MONO, STEREO, etc
	inline const uint8 getNumChannels() const
	{
		return m_numChannels;
	}

	//How many bytes per samples -> bitdepth / 8
	inline const uint8 getBytesPerSample() const
	{
		return m_bitDepth / 8;
	}

	//What's the sample rate
	inline const uint32 getSampleRate() const
	{
		return m_sampleRate;
	}

	//The audioDataBuffer
	inline const unsigned char* getAudioData() const
	{
		return m_pDataBuffer;
	}

	//Number of frames
	//Frame: frame = numSamples / numChannels
	inline const uint32 getNumFrames() const
	{
		return static_cast<uint32>(getNumSamples() / getNumChannels());
	}

	const size_t& getDataBufferSize() const
	{
		return m_dataBufferSize;
	}

	Vector<float> getAmplitudeSamples();

	Vector<float> getDBSamples();

	float getSample(size_t sample);

protected:
	friend class AudioAPI;
	friend class SoundEngine;

	WAVEFORMATEXTENSIBLE m_waveFile{ 0 };
	XAUDIO2_BUFFER m_buffer{ 0 };

	uint8 m_numChannels;
	uint32 m_sampleRate;
	uint8 m_bitDepth;
	size_t m_dataBufferSize;
	String m_name;
	String m_filePath;

	unsigned char* m_pDataBuffer = nullptr;
};

