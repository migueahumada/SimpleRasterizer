#include "Audio.h"
#include "Submix.h"
#include <cstdint>  // for int16_t, int32_t, etc.
#include <limits>

Audio::Audio(const String& name, 
						 const String& filePath) 
	: m_name(name), 
		m_filePath(filePath)
{
	load(filePath.c_str());

	
}

Audio::~Audio()
{
	if (m_pDataBuffer)
	{
		delete[] m_pDataBuffer;
		m_pDataBuffer = nullptr;
	}

}

void Audio::load(const char* filename)
{
	fstream file(filename, std::ios::in | std::ios::beg | std::ios::binary);
	if (!file.is_open())
		return;

	unsigned long temp_buffer;

	file.read(reinterpret_cast<char*>(&temp_buffer), sizeof(unsigned long));
	if (temp_buffer != fourccRIFF)
		return;

	file.read(reinterpret_cast<char*>(&temp_buffer), sizeof(unsigned long));

	file.read(reinterpret_cast<char*>(&temp_buffer), sizeof(unsigned long));
	if (temp_buffer != fourccWAVE)
		return;

	while (file.peek() != EOF)
	{
		unsigned long chunkID;
		unsigned long chunkSize;

		//Read chunk ID
		file.read(reinterpret_cast<char*>(&chunkID), sizeof(unsigned long));

		file.read(reinterpret_cast<char*>(&chunkSize), sizeof(unsigned long));

		

		switch (chunkID)
		{
		case fourccFMT:

			file.read(reinterpret_cast<char*>(&m_waveFile), chunkSize);
			break;
		case fourccDATA:
			m_pDataBuffer = new unsigned char[chunkSize];
			file.read(reinterpret_cast<char*>(m_pDataBuffer), chunkSize);
			m_buffer.pAudioData = m_pDataBuffer;
			m_buffer.AudioBytes = chunkSize;
			
			m_dataBufferSize = chunkSize;
			break;

		default:

			file.seekg(chunkSize, std::ios::cur);
			break;
		}

	}
	

	m_buffer.Flags = XAUDIO2_END_OF_STREAM;

	m_numChannels = m_waveFile.Format.nChannels;
	m_sampleRate = m_waveFile.Format.nSamplesPerSec;
	m_bitDepth = m_waveFile.Format.wBitsPerSample;
	
	file.close();
}

Vector<float> Audio::getAmplitudeSamples()
{
	Vector<float> samples;
	samples.resize(getNumSamples());

	assert(getNumSamples() > 0);

	for (size_t i = 0; i < getNumSamples(); ++i)
	{
		samples[i] = getSample(i);
	}

	return samples;
}

Vector<float> Audio::getDBSamples()
{
	Vector<float> samples;
	samples.resize(getNumSamples());

	assert(getNumSamples() > 0);

	for (size_t i = 0; i < getNumSamples(); ++i)
	{
		samples[i] = 20.0f * std::log10(getSample(i));
	}

	return samples;
}

float Audio::getSample(size_t sample)
{
	uint8* byteAudioData = reinterpret_cast<uint8*>(m_pDataBuffer);

	float sampleValue = 0.0f;
	
	if (m_bitDepth == 16)
	{
		int16 intValue = *reinterpret_cast<int16*>(&byteAudioData[sample * 2]);
		sampleValue = intValue / static_cast<float>((std::numeric_limits<int16>::max)());
	}

	if (m_bitDepth == 32)
	{
		int32 intValue = *reinterpret_cast<int32*>(&byteAudioData[sample * 4]);
		sampleValue = intValue / static_cast<float>((std::numeric_limits<int32>::max)());
	}

	return sampleValue;
}




//NumSamples = sampleRate * duration.
//NumSamples = 


/*
*	Stereo 48kHz 16bits
	- SR = 48000
	- BD = 16
	- CH = 2
	- Data Size (bytes) = SR * CH * BD/8
	- NumSample = DataSize / NumChannles * BD/8
*/

