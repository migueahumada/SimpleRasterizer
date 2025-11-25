#include "SoundEngine.h"
#include "Submix.h"
#include "Audio.h"
#include "Master.h"
#include "Channel.h"


void SoundEngine::OnStartUp()
{
  //Init Audio Device
  g_audioAPI().Init();

  

  //Start Main Output
  m_pMasterOutput = g_audioAPI().CreateMaster(32,48000);

  if (!m_pMasterOutput)
  {
    return;
  }

  //Init3DAudio
  g_audioAPI().Init3DAudio(m_pMasterOutput);

  SPtr<Audio> pAudio = g_audioAPI().CreateAudio("Audio x",
    "D:/Coding/C++/SimpleRasterizer/audio/Audio_001.wav");
  SPtr<Audio> pAudio2 = g_audioAPI().CreateAudio("Draggin Rock",
    "D:/Coding/C++/SimpleRasterizer/audio/DraggingRock.wav");
  SPtr<Audio> pAudio3 = g_audioAPI().CreateAudio("Laser Gun",
    "D:/Coding/C++/SimpleRasterizer/audio/LaserGun.wav");

  SPtr<Channel> pChannel = g_audioAPI().CreateChannel(pAudio, 2, 48000);
  SPtr<Channel> pChannel2 = g_audioAPI().CreateChannel(pAudio2, 2, 48000);
  SPtr<Channel> pChannel3 = g_audioAPI().CreateChannel(pAudio3, 2, 48000);

  SPtr<Submix> pSubmix = g_audioAPI().CreateSubmix();

  m_mapChannels.insert({"Channel1", pChannel});
  m_mapChannels.insert({ "Channel2", pChannel2 });
  m_mapChannels.insert({ "Channel3", pChannel3 });

  m_mapAudios.insert({ "Audio1", pAudio });
  m_mapAudios.insert({ "Audio2", pAudio2 });
  m_mapAudios.insert({ "Audio3", pAudio3 });

  m_mapSubmixes.insert({"Submix1", pSubmix});

  m_mapChannels.find("Channel1")->second->Route(m_mapSubmixes.find("Submix1")->second);
  m_mapChannels.find("Channel2")->second->Route(m_mapSubmixes.find("Submix1")->second);
  m_mapChannels.find("Channel3")->second->Route(m_mapSubmixes.find("Submix1")->second);
  
  m_mapSubmixes.find("Submix1")->second->m_pSubmixVoice->SetVolume(0.01f);

  Play(m_mapChannels.find("Channel1")->second, 0.01f);
  Play(m_mapChannels.find("Channel2")->second,0.01f);
  Play(m_mapChannels.find("Channel3")->second, 0.01f);
}

void SoundEngine::OnShutdown()
{
}

void SoundEngine::Play(const WPtr<Channel>& channel, float volume)
{
  if (channel.expired())
  {
    return;
  }

  auto CHANNEL = channel.lock();

  CHANNEL->m_pSourceVoice->SetVolume(volume);

  CHANNEL->m_pSourceVoice->Start(0); 
}

void SoundEngine::Update()
{
  
  Vector<ChannelMap::iterator> pStoppedChannels;
  for (auto it = m_mapChannels.begin(), 
       itEnd = m_mapChannels.end(); 
       it != itEnd; ++it)
  {
    int asdf = it->second.use_count();
    if (!it->second->isPlaying())
    {
      pStoppedChannels.push_back(it);
    }
    
  }

  //Elimiante all stopped Channels
  for (auto& it : pStoppedChannels)
  {
    m_mapChannels.erase(it);
  }
  
}

bool SoundEngine::CreateAudio(const String& name, 
                              const String& filePath)
{
  SPtr<Audio> pAudio = g_audioAPI().CreateAudio(name,filePath);
  if (!pAudio)
  {
    return false;
  }

  m_mapAudios.insert({name,pAudio});

  return true;

}

bool SoundEngine::CreateChannel(const String& name,
                                const SPtr<Audio>& pAudio,
                                int32 numChannels,
                                int32 inSampleRate)
{
  SPtr<Channel> pChannel = g_audioAPI().CreateChannel(pAudio,
                                                      numChannels, 
                                                      inSampleRate);
  if (!pChannel)
  {
    return false;
  }

  m_mapChannels.insert({name, pChannel});

  return true;
}

SoundEngine& g_soundEngine()
{
  
  return SoundEngine::GetInstance();
}
