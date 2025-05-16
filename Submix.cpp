#include "Submix.h"
#include "Audio.h"


Submix::Submix(unsigned int inputChannels, unsigned int inputSampleRate) 
              : m_inputChannels(inputChannels),
                m_inputSampleRate(inputSampleRate)
{
  m_sends.SendCount = 0;
  m_sends.pSends = m_sendList.data();

  m_fxs.EffectCount = 0;
  m_fxs.pEffectDescriptors = m_fxsList.data();
}

Submix::~Submix()
{
  m_pSubmixVoice->DestroyVoice();
}

void Submix::RouteToSubmix(const SPtr<Submix>& destSubmix, unsigned int flags)
{
  
  if (!destSubmix->m_pSubmixVoice)
  {
    MessageBox(nullptr,L"THE SUBMIX VOICE IS NULLPTR",L"ERROR", S_OK);
    return;
  }

  XAUDIO2_SEND_DESCRIPTOR sendDesc {0};

  sendDesc.pOutputVoice = destSubmix->m_pSubmixVoice;
  sendDesc.Flags = flags;

  m_sends.SendCount++;

  m_sendList.push_back(sendDesc);
  
  printf("Routing submix done successfully!");
}

void Submix::RouteToAudio(const SPtr<Audio>& audio, unsigned int flags)
{

  if (!audio->getSourceVoice())
  {
    MessageBox(nullptr, L"Routing couldn't be done. Sourcevoice is nullptr",
      L"ERROR", S_OK);
    return;
  }

  XAUDIO2_SEND_DESCRIPTOR sendDesc{ 0 };
  sendDesc.pOutputVoice = audio->getSourceVoice();
  sendDesc.Flags = flags;

  m_sends.SendCount++;
  m_sendList.push_back(sendDesc);


  printf("Routing simple audio, done successfully!");

}





