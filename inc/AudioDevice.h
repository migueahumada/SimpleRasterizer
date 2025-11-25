#pragma once

#include <xaudio2.h>
#include "HelperMacros.h"

class AudioDevice
{
public:
	AudioDevice() = default;

#ifdef XAUDIO
	IXAudio2* m_pAbstractAudioDevice;
#endif // XAUDIO2




};

