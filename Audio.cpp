#include "Audio.h"

Audio::Audio(const String& name, const String& filePath) : m_name(name)
{
	load(filePath.c_str());
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
			break;

		default:

			file.seekg(chunkSize, std::ios::cur);
			break;
		}

	}

	m_buffer.Flags = XAUDIO2_END_OF_STREAM;

	file.close();
}

