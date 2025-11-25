#pragma once

#include <xaudio2.h>
#include "HelperMacros.h"
#include "Audio.h"

class Audio;
class Channel;

class Submix
{

public:
  Submix(unsigned int inputChannels = 1, 
         unsigned int inputSampleRate = 44100);
  virtual ~Submix();

  unsigned int getInputChannels() const{
    return m_inputChannels;
  }

  unsigned int getInputSampleRate() const{ 
    return m_inputSampleRate;
  }

  //void Route(const WPtr<Channel>& channel, unsigned int flags = 0);

protected:
  friend class AudioAPI;
  friend class SoundEngine;
  friend class Channel;

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

