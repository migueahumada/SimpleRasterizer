#pragma once

#include <xaudio2.h>
#include "HelperMacros.h"

class Master
{

public:
  Master();
  virtual ~Master()
  {
    m_pMasterVoice->DestroyVoice();
  }
  
  IXAudio2MasteringVoice* getMasterVoice()
  {
    return m_pMasterVoice;
  }

  uint8 getOutputNumChannels()
  {
    XAUDIO2_VOICE_DETAILS voiceDetails{ 0 };
    m_pMasterVoice->GetVoiceDetails(&voiceDetails);
    return static_cast<uint8>(voiceDetails.InputChannels);
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

