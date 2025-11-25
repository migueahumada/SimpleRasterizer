#pragma once
#include "HelperMacros.h"
#include <xaudio2.h>
#include <xaudio2fx.h>
#include <x3daudio.h>

class Submix;
class VoiceCallback;
class Audio;

class Channel
{
 public:
  Channel(unsigned int inputChannels, 
          unsigned int inputSampleRate);
  ~Channel();
  
  inline unsigned int getInputChannels() const {
    return m_inputChannels;
  }

  inline unsigned int getInputSampleRate() const {
    return m_inputSampleRate;
  }

  void SubmitAudio(const WPtr<Audio>& pAudio);

  void Route(const WPtr<Submix>& channel,
             unsigned int flags = 0);
  
  bool isPlaying();


 private:
  friend class AudioAPI;
  friend class SoundEngine;
  IXAudio2SourceVoice* m_pSourceVoice = nullptr;

  unsigned int m_inputChannels;
  unsigned int m_inputSampleRate;
  
  XAUDIO2_VOICE_STATE m_voiceState{0};

  //Sends 
  XAUDIO2_VOICE_SENDS m_sends{ 0 };
  Vector<XAUDIO2_SEND_DESCRIPTOR> m_sendList;

  //FX
  XAUDIO2_EFFECT_CHAIN m_fxs{ 0 };
  Vector<XAUDIO2_EFFECT_DESCRIPTOR> m_fxsList;

  bool m_isSpatialized = false;
  X3DAUDIO_EMITTER m_emitter{0};
  X3DAUDIO_DSP_SETTINGS m_dspSettings{0};
};

