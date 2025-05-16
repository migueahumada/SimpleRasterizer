#pragma once
#include "HelperMacros.h"

#include <xaudio2.h>



class Audio;
class Submix;
class Master;
class Camera;
class VoiceCallback;

class AudioAPI
{
public:
  explicit AudioAPI(void* pHwnd);
  ~AudioAPI();

  void Init();

  SPtr<Audio> CreateSoundEffect(const String& name, 
                                const String& filepath,
                                const SPtr<VoiceCallback>& pCallback = nullptr);

  SPtr<Submix> CreateSubmix(unsigned int inputChannels,
                            unsigned int inputSampleRate);

  SPtr<Master> CreateMaster(unsigned int inChannels = XAUDIO2_DEFAULT_CHANNELS,
                            unsigned int inSampleRate = XAUDIO2_DEFAULT_SAMPLERATE,
                            unsigned int flags = 0);

  template<typename T, typename S>
  void RouteTo(const WPtr<T>& from, const WPtr<S>& to, unsigned int flags);
  
  void SetCameraListener(Camera& camera);

  void Play(const WPtr<Audio>& audio, float volume);

  void Update();

private:

 // UnorderedMap<String, SPtr<Audio>> m_audioMap;

  IXAudio2* m_pXAudio2 = nullptr;
  
  IXAudio2MasteringVoice* m_XAudio2MasteringVoice = nullptr;
  HWND m_pHwnd = nullptr;
};

/// <summary>
/// Routea audio entre submixes/submixes y audio/submix
/// </summary>
/// <typeparam name="T">Submix/Audio</typeparam>
/// <typeparam name="S">Submix</typeparam>
/// <param name="from">submix o audio fuente</param>
/// <param name="to">submix destino, no puede ser audio</param>
/// <param name="flags"> banderas extras de xaudio </param>
template<typename T, typename S>
void AudioAPI::RouteTo(const WPtr<T>& from, const WPtr<S>& to, unsigned int flags)
{
  static_assert(std::is_base_of<Audio,T>::value || 
                std::is_base_of<Submix,T>::value,
                "T must be deribed from Audio or Channel");
  static_assert(std::is_base_of<Submix, S>::value,
                "S must be deribed from Audio or Channel");
                
}

//template <typename T>
//void Bus::Route(const WPtr<T>& voice, unsigned int flags)
//{
//  static_assert(std::is_base_of<Audio, T>::value ||
//    std::is_base_of<Submix, T>::value,
//    "T should be Audio or Submix type.");
//
//  if (voice.expired())
//  {
//    return;
//  }
//
//  SPtr<T> pVoice = voice.lock();
//
//  XAUDIO2_SEND_DESCRIPTOR sendDesc;
//  memset(&sendDesc, 0, XAUDIO2_SEND_DESCRIPTOR);
//
//  sendDesc.Flags = flags;
//  sendDesc.pOutputVoice = pVoice->getSubmixVoice();
//
//  m_sendsList.push_back(sendDesc);
//  m_sends.SendCount = m_sendsList.size();
//  m_sends.pSends = m_sendsList.data();
//
//  HRESULT hr = pVoice->SetOutputVoices(&m_sends);
//  if (FAILED(hr))
//  {
//    MessageBox(nullptr, L"Error routing bus", L"ERROR", S_OK);
//    return;
//  }
//
//}
