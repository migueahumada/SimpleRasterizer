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

  SPtr<Audio> pAudio = g_audioAPI().CreateAudio("Sounds",
    "D:/Coding/C++/SimpleRasterizer/audio/Audio_001.wav");
  if (!pAudio)
  {
    return;
  }
  SPtr<Audio> pAudio2 = g_audioAPI().CreateAudio("Sounds",
    "D:/Coding/C++/SimpleRasterizer/audio/MX_Menu_Loop.wav");
  if (!pAudio2)
  {
    return;
  }

  SPtr<Channel> pChannel = g_audioAPI().CreateChannel(pAudio, 2, 48000);
  if (!pChannel)
  {
    return;
  }
  SPtr<Channel> pChannel2 = g_audioAPI().CreateChannel(pAudio2, 2, 48000);
  if (!pChannel2)
  {
    return;
  }

  SPtr<Submix> pSubmix = g_audioAPI().CreateSubmix();
  if (!pSubmix)
  {
    return;
  }

  //SPtr<Channel> pEmptyChannel = g_audioAPI().CreateChannel(2, 48000);
  //if (!pEmptyChannel)
  //{
  //  return;
  //}

  /*pEmptyChannel->SubmitAudio(pAudio);*/

  m_mapChannels.insert({"Channel1", pChannel});
  m_mapChannels.insert({ "Channel2", pChannel2 });

  m_mapAudios.insert({ "Audio1", pAudio });
  m_mapAudios.insert({ "Audio2", pAudio2 });

  m_mapSubmixes.insert({"Submix1", pSubmix});

  m_mapChannels.find("Channel1")->second->Route(m_mapSubmixes.find("Submix1")->second);
  m_mapChannels.find("Channel2")->second->Route(m_mapSubmixes.find("Submix1")->second);
  
  m_mapSubmixes.find("Submix1")->second->m_pSubmixVoice->SetVolume(5.0f);

  Play(m_mapChannels.find("Channel1")->second, 0.2f);
  Play(m_mapChannels.find("Channel2")->second,0.2f);

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
