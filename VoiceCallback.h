#pragma once
#include <xaudio2.h>
#include <stdio.h>

class AudioAPI;

class VoiceCallback : public IXAudio2VoiceCallback
{
public:
  VoiceCallback();
  ~VoiceCallback();

  HANDLE hBufferEndEvent = nullptr;

  void OnStreamEnd() override;

  //Unused methods are stubs
  virtual void OnVoiceProcessingPassEnd() override { }
  virtual void OnVoiceProcessingPassStart(UINT32 SamplesRequired) override {    }
  virtual void OnBufferEnd(void* pBufferContext) override{ }
  virtual void OnBufferStart(void* pBufferContext) override{
    printf("AUDIO STARTED!");
  }
  virtual void OnLoopEnd(void* pBufferContext) override{    }
  virtual void OnVoiceError(void* pBufferContext, HRESULT Error) override { }

  friend class AudioAPI;
private:
};

