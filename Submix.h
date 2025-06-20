#pragma once

#include <xaudio2.h>
#include "HelperMacros.h"

class Audio;

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

  void RouteToSubmix(const WPtr<Submix>& destSubmix, unsigned int flags = 0);
  void RouteToAudio(const WPtr<Audio>& audio, unsigned int flags = 0);

  IXAudio2SubmixVoice* getSubmixVoice() const {
    return m_pSubmixVoice;
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

