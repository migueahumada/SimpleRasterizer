#pragma once
#include "HelperMacros.h"

#include <xaudio2.h>



class Audio;
class Submix;
class Camera;

class AudioAPI
{
public:
  explicit AudioAPI(void* pHwnd);
  ~AudioAPI();

  void Init();

  SPtr<Audio> CreateSoundEffect(const String& name, 
                                const String& filepath,
                                IXAudio2VoiceCallback* pCallback = nullptr,
                                const SPtr<Submix> submix = nullptr);

  SPtr<Submix> CreateSubmix(unsigned int inputChannels,
                            unsigned int inputSampleRate);

  template<typename T>
  SPtr<T> routeTo(const SPtr<T>& from, const SPtr<T>& to);
  
  
  void SetCameraListener(Camera& camera);

  void Play(SPtr<Audio>& audio, float volume);

  void Update();

private:

 // UnorderedMap<String, SPtr<Audio>> m_audioMap;

  IXAudio2* m_pXAudio2 = nullptr;
  
  IXAudio2MasteringVoice* m_XAudio2MasteringVoice = nullptr;
  HWND m_pHwnd = nullptr;
};

template<typename T>
SPtr<T> AudioAPI::routeTo(const SPtr<T>& from, const SPtr<T>& to)
{
  static_assert(std::is_base_of<Audio,T>::value, "T must be derived from Audio");

}
