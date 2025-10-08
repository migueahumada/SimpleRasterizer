#pragma once
#include "Module.h"
#include "AudioDevice.h"
#include "AudioAPI.h"

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

  void Play(const WPtr<Channel>& channel, float volume = 0.0f);

  void Update();

  ChannelMap& GetChannels()
  {
    return m_mapChannels;
  }

 protected:

  SPtr<Master> m_pMasterOutput;

  ChannelMap m_mapChannels;
  AudioMap m_mapAudios;
  SubmixMap m_mapSubmixes;
};

SoundEngine& g_soundEngine();



