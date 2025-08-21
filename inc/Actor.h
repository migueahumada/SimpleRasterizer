#pragma once
#include "MathObjects.h"
#include "SceneObject.h"

#include "Renderer.h"

class Actor : public SceneObject
{

public:
  Actor() = default;
  virtual ~Actor() = default;

  virtual void Init();
  virtual void FixedUpdate();
  virtual void Update(float deltaTime);
  virtual void Render();

  Transform getLocalTransform() const{
    return m_localTransform;
  }

  Transform getWorldTransform() const{
    return m_worldTransform;
  }
  
  ShadingState::E getShadingState() const{
    return m_shadingState;
  }
  
  void setShadingState(ShadingState::E newShadingState)
  {
    m_shadingState = newShadingState;
  }

private:
  
protected:
  
  void UpdateTransform();

  Transform m_localTransform;
  Transform m_worldTransform;

  ShadingState::E m_shadingState {ShadingState::DEFERRED};
  
  
};

