#pragma once
#include "MathObjects.h"
#include "SceneObject.h"

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

private:
  
protected:
  
  void UpdateTransform();

  Transform m_localTransform;
  Transform m_worldTransform;


  
  
};

