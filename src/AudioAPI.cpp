
#include "AudioAPI.h"
#include "Audio.h"
#include "Submix.h"
#include "Master.h"
#include "Camera.h"
#include "VoiceCallback.h"
#include <assert.h>
#include "Channel.h"

AudioAPI::AudioAPI()
{
  
}

AudioAPI::~AudioAPI()
{
  SAFE_RELEASE(m_pAudioDevice);
}

void AudioAPI::Init()
{

  HRESULT hr;

  UINT flags = 0;
#if defined (_DEBUG)
  flags |= XAUDIO2_DEBUG_ENGINE;
#endif // defined (_DEBUG)

  //audioDevice.;
  
  hr = XAudio2Create(&m_pAudioDevice, flags);
  if (FAILED(hr))
  {
    MessageBox(nullptr, L"Couldn't create XAudio2", L"Error", MB_OK);
    return;
  }

  hr = m_pAudioDevice->StartEngine();
  if (FAILED(hr))
  {
    MessageBox(nullptr,L"Couldn't start XAudio2", L"Error", MB_OK);
    return;
  }



}

SPtr<Audio> AudioAPI::CreateAudio(const String& name, 
                                  const String& filepath,
                                  const SPtr<VoiceCallback>& pCallback)
{

  SPtr<Audio> pAudio = make_shared<Audio>(name, filepath);
  if (!pAudio)
  {
    MessageBox(nullptr, L"Failed to create Audio", L"Error", MB_OK);
    return nullptr;
  }

  return pAudio;

}

SPtr<Channel> AudioAPI::CreateChannel(const SPtr<Audio>& pAudio, 
                                      uint32 inputChannels,
                                      uint32 inputSampleRate)
{
  HRESULT hr;

  SPtr<Channel> pChannel = make_shared<Channel>(inputChannels,
                                                inputSampleRate);

  if (!pChannel)
  {
    MessageBox(nullptr, L"Failed to create Channel", L"Error", MB_OK);
    return nullptr;
  }

  hr = m_pAudioDevice->CreateSourceVoice(&pChannel->m_pSourceVoice,
                                         (WAVEFORMATEX*)&pAudio->m_waveFile,
                                         0);
  
  if (pAudio)
  {
    pChannel->m_pSourceVoice->SubmitSourceBuffer(&pAudio->m_buffer);
  }

  if (FAILED(hr))
  {
    MessageBox(nullptr, L"Failed to create Channel SourceVoice", L"Error", MB_OK);
    return nullptr;
  }

  return pChannel;

}

SPtr<Channel> AudioAPI::CreateChannel(uint32 inputChannels, uint32 inputSampleRate)
{
  HRESULT hr;

  WAVEFORMATEX wfx;
  wfx.wFormatTag = WAVE_FORMAT_PCM;
  wfx.nChannels = inputChannels;
  wfx.nSamplesPerSec = inputSampleRate;
  wfx.wBitsPerSample = 16;
  wfx.nBlockAlign = (wfx.wBitsPerSample / 8) * wfx.nChannels;
  wfx.nAvgBytesPerSec = wfx.nBlockAlign * wfx.nSamplesPerSec;

  SPtr<Channel> pChannel = make_shared<Channel>(inputChannels,
    inputSampleRate);

  if (!pChannel)
  {
    MessageBox(nullptr, L"Failed to create Channel", L"Error", MB_OK);
    return nullptr;
  }

  hr = m_pAudioDevice->CreateSourceVoice(&pChannel->m_pSourceVoice,
                                         &wfx,
                                         0);

  //hr = pChannel->m_pSourceVoice->SubmitSourceBuffer(&audioBuffer.buffer);


  if (FAILED(hr))
  {
    MessageBox(nullptr, L"Failed to create empty Channel", L"Error", MB_OK);
    return nullptr;
  }

  return pChannel;
}

//Problem is that you need an existent channel before constructing a submix. 
SPtr<Submix> AudioAPI::CreateSubmix(uint32 inputChannels,
                                    uint32 inputSampleRate)
{
  SPtr<Submix> pSubmix = make_shared<Submix>(inputChannels,inputSampleRate);

  if (!pSubmix)
  {
    MessageBox(nullptr, L"Failed to create Submix object", L"Error", MB_OK);
    return nullptr;
  }

  HRESULT hr = m_pAudioDevice->CreateSubmixVoice(&pSubmix->m_pSubmixVoice,
                                                 inputChannels,
                                                 inputSampleRate);
  if (FAILED(hr))
  {
    MessageBox(nullptr, L"Failed to create XAudio2 Submix Voice", L"Error", MB_OK);
    return nullptr;
  }

  return pSubmix;
}



SPtr<Master> AudioAPI::CreateMaster(uint32 inChannels, 
                                    uint32 inSampleRate,
                                    uint32 flags)
{

  SPtr<Master> pMaster = make_shared<Master>();

  HRESULT hr = m_pAudioDevice->CreateMasteringVoice(&pMaster->m_pMasterVoice,
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

void AudioAPI::SubmitAudioBuffer(Channel* channel, const WPtr<Audio>& pAudio)
{
  if (pAudio.expired())
  {
    return;
  }

  HRESULT hr;

  auto AUDIO = pAudio.lock();
  
  hr = channel->m_pSourceVoice->SubmitSourceBuffer(&AUDIO->m_buffer);

  if (FAILED(hr))
  {
    MessageBox(nullptr, L"Failed to Submit Audio Buffer", L"Error", MB_OK);
    return;
  }
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





void AudioAPI::Update()
{

}

void AudioAPI::OnStartUp()
{
}

void AudioAPI::OnShutdown()
{
}

AudioAPI& g_audioAPI()
{
  return AudioAPI::GetInstance();
}
