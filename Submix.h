#pragma once

#include <xaudio2.h>
#include "HelperMacros.h"

class Submix
{
public:
  Submix(unsigned int inputChannels = 0, 
         unsigned int inputSampleRate = 44100);
  virtual ~Submix() = default;

  unsigned int getInputChannels() const{
    return m_inputChannels;
  }

  unsigned int getInputSampleRate() const{ 
    return m_inputSampleRate;
  }


protected:
  friend class AudioAPI;

  unsigned int m_inputChannels;
  unsigned int m_inputSampleRate;

  IXAudio2SubmixVoice* m_pSubmixVoice = nullptr;

  //Sends 
  XAUDIO2_VOICE_SENDS m_sends{0};
  Vector<XAUDIO2_SEND_DESCRIPTOR> m_sendList;
  
  //FX
  XAUDIO2_EFFECT_CHAIN m_fxs{0};
  Vector<XAUDIO2_EFFECT_DESCRIPTOR> m_fxsList;

};

