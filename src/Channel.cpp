#include "Channel.h"
#include "Audio.h"
#include "Submix.h"
#include "AudioAPI.h"
#include "SoundEngine.h"

Channel::Channel(unsigned int inputChannels, 
                 unsigned int inputSampleRate)
  : m_inputChannels(inputChannels),
    m_inputSampleRate(inputSampleRate)
{
  if (m_isSpatialized)
  {
    m_emitter.ChannelCount = 1;
    m_emitter.CurveDistanceScaler = m_emitter.DopplerScaler = 1.0f;

    FLOAT32* matrix = new FLOAT32[g_soundEngine().getMainOutputNumChannels()];
    m_dspSettings.SrcChannelCount = 1;
    m_dspSettings.DstChannelCount = g_soundEngine().getMainOutputNumChannels();
    m_dspSettings.pMatrixCoefficients = matrix;
  }
  
}

Channel::~Channel()
{
  m_pSourceVoice->DestroyVoice();
}


void Channel::SubmitAudio(const WPtr<Audio>& pAudio)
{
  
  if (pAudio.expired())
  {
    return;
  }

  auto AUDIO = pAudio.lock();

  g_audioAPI().SubmitAudioBuffer(this,pAudio);
}

void Channel::Route(const WPtr<Submix>& submix, unsigned int flags)
{
  if (submix.expired())
  {
    return;
  }

  auto SUBMIX = submix.lock();

	XAUDIO2_SEND_DESCRIPTOR sendDesc{0};
	memset(&sendDesc,0,sizeof(XAUDIO2_SEND_DESCRIPTOR));
	
	sendDesc.Flags = flags;
	sendDesc.pOutputVoice = SUBMIX->m_pSubmixVoice;

	m_sendList.push_back(sendDesc);
	m_sends.SendCount = static_cast<uint32>(m_sendList.size());
	m_sends.pSends = m_sendList.data();
	
	HRESULT hr = m_pSourceVoice->SetOutputVoices(&m_sends);

  printf("Routing submix done successfully!");
}

bool Channel::isPlaying()
{
  m_pSourceVoice->GetState(&m_voiceState);
  if (m_voiceState.BuffersQueued < 1)
  {
    return false;
  }
  else
  {
    return true;
  }
}


