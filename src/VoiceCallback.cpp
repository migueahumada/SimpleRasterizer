#include "VoiceCallback.h"

VoiceCallback::VoiceCallback()
{
  CreateEvent(nullptr, false, false, nullptr);
}

VoiceCallback::~VoiceCallback()
{
  CloseHandle(hBufferEndEvent);
}

void VoiceCallback::OnStreamEnd()
{
  SetEvent(hBufferEndEvent);
}
