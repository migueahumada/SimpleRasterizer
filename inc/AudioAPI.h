#pragma once

#include "HelperMacros.h"
#include <xaudio2.h>
#include "Module.h"
#include "AudioDevice.h"


class Audio;
class Submix;
class Master;
class Camera;
class VoiceCallback;
class Actor;
class AudioDevice;
class Channel;

class AudioAPI : public Module<AudioAPI>
{
public:
  AudioAPI();
  virtual ~AudioAPI();

  void Init();

  SPtr<Audio> CreateAudio(const String& name,
                          const String& filepath,
                          const SPtr<VoiceCallback>& pCallback = nullptr);

  SPtr<Channel> CreateChannel(const SPtr<Audio>& pAudio = nullptr,
                              uint32 inputChannels = 2,
                              uint32 inputSampleRate = 48000);

  SPtr<Channel> CreateChannel(uint32 inputChannels = 2,
                              uint32 inputSampleRate = 48000);
  
  SPtr<Submix> CreateSubmix(uint32 inputChannels = 2,
                            uint32 inputSampleRate = 48000);

  

  SPtr<Master> CreateMaster(uint32 inChannels = XAUDIO2_DEFAULT_CHANNELS,
                            uint32 inSampleRate = XAUDIO2_DEFAULT_SAMPLERATE,
                            uint32 flags = 0);

  void SubmitAudioBuffer(Channel* channel, const WPtr<Audio>& pAudio);

  template<typename T, typename S>
  void RouteTo(const SPtr<T>& from, const SPtr<S>& to, unsigned int flags);
  
  void Init3DAudio(WPtr<Master> pMaster);

  void SetListener(WPtr<Actor> lsitenerActor);

  

  void Update();

private:
  void OnStartUp() override;
  void OnShutdown() override;
  
 // UnorderedMap<String, SPtr<Audio>> m_audioMap;

#ifdef XAUDIO
  IXAudio2* m_pAudioDevice = nullptr;
#endif // XAUDIO

#ifdef OPENAL
  OPEN_AL m_pAudioDevice;
#endif // OPENAL



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
void AudioAPI::RouteTo(const SPtr<T>& from, const SPtr<S>& to, unsigned int flags)
{
  static_assert(std::is_base_of<Audio,T>::value || 
                std::is_base_of<Submix,T>::value,
                "T must be deribed from Audio or Channel");
  static_assert(std::is_base_of<Submix, S>::value,
                "S must be deribed from Audio or Channel");
 

  auto FROM = from.lock();
  auto TO = to.lock();

  XAUDIO2_SEND_DESCRIPTOR sendDesc{0};
  memset(&sendDesc, 0, sizeof(XAUDIO2_SEND_DESCRIPTOR));

  if (typeid(T) == Audio)
  {
    printf("This is audio");
  }
}

AudioAPI& g_audioAPI();