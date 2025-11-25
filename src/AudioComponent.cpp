#include "AudioComponent.h"
#include "AudioAPI.h"
#include "Audio.h"
#include "Channel.h"
#include "SoundEngine.h"

AudioComponent::AudioComponent(const String& name, 
                               const String& filePath)
{
  g_soundEngine().CreateAudio(name, filePath);

  
  auto it = g_soundEngine().GetAudios().find(name);

  auto channelName = it->first;
  auto pAudio = it->second;

  g_soundEngine().CreateChannel(channelName, pAudio, 
                                pAudio->getNumChannels(),
                                pAudio->getSampleRate());

  m_audio = g_soundEngine().GetAudios().find(name)->second;
  m_channel = g_soundEngine().GetChannels().find(name)->second;
  
}

void AudioComponent::Init()
{
  
}

void AudioComponent::Update()
{
  
}

bool AudioComponent::Play()
{
  if (m_channel.expired() && m_audio.expired())
  {
    return false;
  }


  g_soundEngine().Play(m_channel,0.1f);

  return true;
}
