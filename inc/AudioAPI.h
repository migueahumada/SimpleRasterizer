#pragma once
#include "HelperMacros.h"

#include <xaudio2.h>
#include "Module.h"


class Audio;
class Submix;
class Master;
class Camera;
class VoiceCallback;
class Actor;

class AudioAPI : public Module<AudioAPI>
{
public:
  AudioAPI();
  virtual ~AudioAPI();

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
  
  void Init3DAudio(WPtr<Master> pMaster);

  void SetListener(WPtr<Actor> lsitenerActor);

  void Play(const WPtr<Audio>& audio, float volume);

  void Update();

private:
  void OnStartUp() override;
  void OnShutdown() override;
  
 // UnorderedMap<String, SPtr<Audio>> m_audioMap;

  IXAudio2* m_pXAudio2 = nullptr;

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

AudioAPI& g_audioAPI();