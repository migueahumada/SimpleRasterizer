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

  //SPtr<Channel> pEmptyChannel = g_audioAPI().CreateChannel(2, 48000);
  //if (!pEmptyChannel)
  //{
  //  return;
  //}

  /*pEmptyChannel->SubmitAudio(pAudio);*/

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
  
  m_mapSubmixes.find("Submix1")->second->m_pSubmixVoice->SetVolume(5.0f);

  Play(m_mapChannels.find("Channel1")->second, 0.2f);
  Play(m_mapChannels.find("Channel2")->second,0.2f);
  Play(m_mapChannels.find("Channel3")->second, 0.2f);

  //float add = m_mapAudios.find("Audio1")->second->getSample(450);
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

SoundEngine& g_soundEngine()
{
  
  return SoundEngine::GetInstance();
}
