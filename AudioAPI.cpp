
#include "AudioAPI.h"
#include "Audio.h"
#include "Submix.h"
#include "Camera.h"
#include <assert.h>

AudioAPI::AudioAPI(void* pHwnd) : m_pHwnd(reinterpret_cast<HWND>(m_pHwnd))
{
  
}

AudioAPI::~AudioAPI()
{
  m_pHwnd = nullptr;
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
    MessageBox(m_pHwnd, L"Couldn't create XAudio2", L"Error", MB_OK);
    return;
  }

  hr = m_pXAudio2->StartEngine();
  if (FAILED(hr))
  {
    MessageBox(m_pHwnd,L"Couldn't start XAudio2", L"Error", MB_OK);
    return;
  }


  hr = m_pXAudio2->CreateMasteringVoice(&m_XAudio2MasteringVoice);
  if (FAILED(hr))
  {
    MessageBox(m_pHwnd, L"Couldn't start mastering voice", L"Error", MB_OK);
    return;
  }

  //TODO: ################################TERMINAR EL AUDIO 3D################################
  DWORD dwChannelMask;
  hr = m_XAudio2MasteringVoice->GetChannelMask(&dwChannelMask);
  if (FAILED(hr))
  {
    MessageBox(m_pHwnd, L"Couldn't get channel mask", L"Error", MB_OK);
    return;
  }

  X3DAUDIO_HANDLE X3DInstance;
  hr = X3DAudioInitialize(dwChannelMask, X3DAUDIO_SPEED_OF_SOUND, X3DInstance);
  if (FAILED(hr))
  {
    MessageBox(m_pHwnd, L"Couldn't intialize 3DAudio", L"Error", MB_OK);
    return;
  }

}

SPtr<Audio> AudioAPI::CreateSoundEffect(const String& name, 
                                        const String& filepath,
                                        IXAudio2VoiceCallback* pCallback,
                                        const SPtr<Submix> submix)
{

  SPtr<Audio> pSoundEffect = make_shared<Audio>(name, filepath);
  if (!pSoundEffect)
  {
    MessageBox(nullptr, L"Failed to create Sound Effect", L"Error", MB_OK);
    return nullptr;
  }

  if (submix)
  {

    HRESULT hr = m_pXAudio2->CreateSourceVoice(&pSoundEffect->m_pSourceVoice,
      (WAVEFORMATEX*)&pSoundEffect->m_waveFile,0,XAUDIO2_DEFAULT_FREQ_RATIO,
      0,&submix->m_sends,&submix->m_fxs);

  } 
  else
  {
    HRESULT hr = m_pXAudio2->CreateSourceVoice(&pSoundEffect->m_pSourceVoice,
      (WAVEFORMATEX*)&pSoundEffect->m_waveFile);

    if (FAILED(hr))
    {
      MessageBox(nullptr, L"Failed to create Source Voice ", L"Error", MB_OK);
      return nullptr;
    }

    pSoundEffect->m_pSourceVoice->SubmitSourceBuffer(&pSoundEffect->m_buffer);
  }

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

void AudioAPI::SetCameraListener(Camera& camera)
{
  X3DAUDIO_LISTENER listener;
  listener.OrientFront.x = camera.GetForwardVector().x;
  listener.OrientFront.y = camera.GetForwardVector().y;
  listener.OrientFront.z = camera.GetForwardVector().z;

  listener.OrientTop.x = camera.GetUpVector().x;
  listener.OrientTop.y = camera.GetUpVector().y;
  listener.OrientTop.z = camera.GetUpVector().z;

  listener.Position.x = camera.getViewMatrix().m[3][0];
  listener.Position.y = camera.getViewMatrix().m[3][1];
  listener.Position.z = camera.getViewMatrix().m[3][2];

  listener.Velocity.x = camera.getVelocity();
  listener.Velocity.y = camera.getVelocity();
  listener.Velocity.z = camera.getVelocity();

  camera.setCameraListener(listener);
  
}

void AudioAPI::Play(SPtr<Audio>& audio, float volume)
{
  if (!audio)
  {
    return;
  }

  audio->m_pSourceVoice->Start(0);
  audio->m_pSourceVoice->SetVolume(volume);

}

void AudioAPI::Update()
{

}
