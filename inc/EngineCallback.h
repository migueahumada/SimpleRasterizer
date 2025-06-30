#pragma once
#include <xaudio2.h>

class EngineCallback : public IXAudio2EngineCallback
{
public:
  virtual void OnProcessingPassEnd() override{}
  virtual void OnProcessingPassStart() override{}
  virtual void OnCriticalError(HRESULT Error) override{}
private:
};

