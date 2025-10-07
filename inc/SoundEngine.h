#pragma once
#include "Module.h"
#include "AudioAPI.h"

class SoundEngine : public Module<SoundEngine>
{
 public:

  SoundEngine() = default;
  virtual ~SoundEngine() = default;

  virtual void OnStartUp() override;
  virtual void OnShutdown() override;

 protected:
};

SoundEngine& g_soundEngine()
{
  return SoundEngine::GetInstance();
}

