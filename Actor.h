#pragma once
#include "MathObjects.h"

class Actor
{

public:
  Actor() = default;
  virtual ~Actor() = default;

  virtual void Init();
  virtual void Update(float dt);
  virtual void Render();
  
protected:

  Transform m_transform;
  
};

