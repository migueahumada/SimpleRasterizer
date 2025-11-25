#pragma once
#include "Component.h"

class Channel;
class Audio;

class AudioComponent : public Component
{
 public:
  
  explicit AudioComponent(const String& name, 
                 const String& filePath);
  virtual ~AudioComponent() = default;

  virtual void Init() override;
  virtual void Update() override;

  bool Play();

protected:
  WPtr<Channel> m_channel;
  WPtr<Audio> m_audio;
};

