#pragma once

#include <xaudio2.h>
#include "HelperMacros.h"

class Submix;
class Audio;


class Bus {

public:
  Bus();
  virtual ~Bus() = default;

protected:
  XAUDIO2_VOICE_SENDS m_sends;
  Vector<XAUDIO2_SEND_DESCRIPTOR> m_sendsList;
};



