#pragma once
#include "HelperMacros.h"
#include "Audio.h"
#include <x3daudio.h>
#include <assert.h>
#include "Camera.h"

class AudioAPI
{
public:
  explicit AudioAPI(void* pHwnd);
  ~AudioAPI();

  void Init();

  SPtr<Audio> CreateSoundEffect(const String& name, 
                                const String& filepath);

  void SetCameraListener(Camera& camera);

  void Play(SPtr<Audio>& audio, float volume);

  void Update();

private:

  IXAudio2* m_pXAudio2 = nullptr;
  IXAudio2MasteringVoice* m_XAudio2MasteringVoice = nullptr;
  void* m_pHwnd = nullptr;
};

