#include "Audio.h"
#include "Submix.h"

Audio::Audio(const String& name, const String& filePath) : m_name(name)
{
	load(filePath.c_str());

	m_sends.pSends = m_sendList.data();
	m_sends.SendCount = m_sendList.size();

	m_effects.pEffectDescriptors = m_effectList.data();
	m_effects.EffectCount = m_effectList.size();
}

Audio::~Audio()
{
	if (m_pDataBuffer)
	{
		delete[] m_pDataBuffer;
		m_pDataBuffer = nullptr;
	}
	m_pSourceVoice->DestroyVoice();
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

void Audio::RouteTo(const WPtr<Submix>& submix, unsigned int flags)
{

	if (submix.expired())
	{
		MessageBox(nullptr, L"Routing couldn't be done. submix is nullptr",
			L"ERROR", S_OK);
		return;
	}

	auto SUBMIX = submix.lock();

	XAUDIO2_SEND_DESCRIPTOR sendDesc{0};
	memset(&sendDesc,0,sizeof(XAUDIO2_SEND_DESCRIPTOR));
	
	sendDesc.Flags = flags;
	sendDesc.pOutputVoice = SUBMIX->getSubmixVoice();

	m_sendList.push_back(sendDesc);
	m_sends.SendCount = m_sendList.size();
	m_sends.pSends = m_sendList.data();
	
	HRESULT hr = m_pSourceVoice->SetOutputVoices(&m_sends);
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"Error setting updating output voices", L"ERROR", S_OK);
		return;
	}

}


