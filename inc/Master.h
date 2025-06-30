#pragma once

#include <xaudio2.h>
#include "HelperMacros.h"

class Master
{

public:
  Master();
  ~Master() = default;
  
  IXAudio2MasteringVoice* getMasterVoice()
  {
    return m_pMasterVoice;
  }



private:
  friend class AudioAPI;

  unsigned int m_inputChannels;
  unsigned int m_inputSampleRate;
  unsigned int m_flags;
  String m_audioDeviceID;
  
  IXAudio2MasteringVoice* m_pMasterVoice;  

  unsigned long m_ChannelMask;
};

