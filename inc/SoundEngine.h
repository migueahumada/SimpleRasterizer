#pragma once
#include "Module.h"
#include "AudioDevice.h"
#include "AudioAPI.h"
#include "Master.h"

using ChannelMap = Map<String, SPtr<Channel>>;
using AudioMap = Map<String, SPtr<Audio>>;
using SubmixMap = Map<String, SPtr<Submix>>;

class SoundEngine : public Module<SoundEngine>
{
 public:

  SoundEngine() = default;
  virtual ~SoundEngine() = default;

  virtual void OnStartUp() override;
  virtual void OnShutdown() override;

  void Play(const WPtr<Channel>& channel, float volume = 1.0f);

  void Update();

  const ChannelMap& GetChannels() const
  {
    return m_mapChannels;
  }

  const AudioMap& GetAudios() const
  {
    return m_mapAudios;
  }

  bool CreateAudio(const String& name,
                   const String& filePath);
  
  bool CreateChannel(const String& name,
                     const SPtr<Audio>& pAudio,
                     int32 numChannels,
                     int32 inSampleRate);

  uint8 getMainOutputNumChannels()
  {
    return m_pMasterOutput->getOutputNumChannels();
  }
 protected:

  SPtr<Master> m_pMasterOutput;

  ChannelMap m_mapChannels;
  AudioMap m_mapAudios;
  SubmixMap m_mapSubmixes;
};

SoundEngine& g_soundEngine();



