
#include "AudioAPI.h"
#include "Audio.h"
#include "Submix.h"
#include "Master.h"
#include "Camera.h"
#include "VoiceCallback.h"
#include <assert.h>

AudioAPI::AudioAPI()
{
  
}

AudioAPI::~AudioAPI()
{
  SAFE_RELEASE(m_pXAudio2);
}

void AudioAPI::Init()
{

  HRESULT hr;

  UINT flags = 0;
#if defined (_DEBUG)
  flags |= XAUDIO2_DEBUG_ENGINE;
#endif // defined (_DEBUG)

  hr = XAudio2Create(&m_pXAudio2, flags);
  if (FAILED(hr))
  {
    MessageBox(nullptr, L"Couldn't create XAudio2", L"Error", MB_OK);
    return;
  }

  hr = m_pXAudio2->StartEngine();
  if (FAILED(hr))
  {
    MessageBox(nullptr,L"Couldn't start XAudio2", L"Error", MB_OK);
    return;
  }



}

SPtr<Audio> AudioAPI::CreateSoundEffect(const String& name, 
                                        const String& filepath,
                                        const SPtr<VoiceCallback>& pCallback)
{

  SPtr<Audio> pSoundEffect = make_shared<Audio>(name, filepath);
  if (!pSoundEffect)
  {
    MessageBox(nullptr, L"Failed to create Sound Effect", L"Error", MB_OK);
    return nullptr;
  }

  HRESULT hr = m_pXAudio2->CreateSourceVoice(&pSoundEffect->m_pSourceVoice,
    (WAVEFORMATEX*)&pSoundEffect->m_waveFile, 0, XAUDIO2_DEFAULT_FREQ_RATIO,
    pCallback.get(), &pSoundEffect->m_sends);

  if (FAILED(hr))
  {
    MessageBox(nullptr, L"Failed to create Source Voice ", L"Error", MB_OK);
    return nullptr;
  }

  pSoundEffect->m_pSourceVoice->SubmitSourceBuffer(&pSoundEffect->m_buffer);

  return pSoundEffect;

}

//TODO: CHECK THE SUBMIX
SPtr<Submix> AudioAPI::CreateSubmix(unsigned int inputChannels,
                                    unsigned int inputSampleRate)
{
  SPtr<Submix> pSubmix = make_shared<Submix>(inputChannels,inputSampleRate);

  HRESULT hr = m_pXAudio2->CreateSubmixVoice(&pSubmix->m_pSubmixVoice,
                                              pSubmix->m_inputChannels,
                                              pSubmix->m_inputSampleRate);
  if (FAILED(hr))
  {
      MessageBox(nullptr, L"Failed to create Submix Voice", L"Error", MB_OK);
      return nullptr;
  }
  return pSubmix;
}

SPtr<Master> AudioAPI::CreateMaster(unsigned int inChannels, 
                                    unsigned int inSampleRate,
                                    unsigned int flags)
{

  SPtr<Master> pMaster = make_shared<Master>();

  HRESULT hr = m_pXAudio2->CreateMasteringVoice(&pMaster->m_pMasterVoice, 
                                                inChannels, inSampleRate, 
                                                flags);
  if (FAILED(hr))
  {
    MessageBox(nullptr, L"Failed to create Master Voice", L"Error", MB_OK);
    return nullptr;
  }

  hr = pMaster->getMasterVoice()->GetChannelMask(&pMaster->m_ChannelMask);
  
  if (FAILED(hr))
  {
    MessageBox(nullptr, L"Failed to get Channel Mask", L"Error", MB_OK);
    return nullptr;
  }

  return pMaster;
}

void AudioAPI::Init3DAudio(WPtr<Master> pMaster)
{
  if (pMaster.expired())
  {
    return;
  }

  auto MASTER = pMaster.lock();

  DWORD dwChannelMask;
  memset(&dwChannelMask,0, sizeof(DWORD));
  HRESULT hr = MASTER->getMasterVoice()->GetChannelMask(&dwChannelMask);
  if (FAILED(hr))
  {
    SHOW_ERROR(L"Couldn't get channel mask");
    return;
  }

  X3DAUDIO_HANDLE X3DInstance;
  hr = X3DAudioInitialize(dwChannelMask, X3DAUDIO_SPEED_OF_SOUND, X3DInstance);
  if (FAILED(hr))
  {
    SHOW_ERROR(L"Couldn't intialize 3DAudio");
    return;
  }
}

void AudioAPI::Play(const WPtr<Audio>& audio, float volume)
{
  if (audio.expired())
  {
    return;
  }
  
  auto AUDIO = audio.lock();
 
  AUDIO->m_pSourceVoice->SetVolume(volume);
  AUDIO->m_pSourceVoice->Start(0);
  

}

void AudioAPI::Update()
{

}
