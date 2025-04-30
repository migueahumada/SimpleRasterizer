#pragma once
#include "HelperMacros.h"

class World
{

public:
  World();
  virtual ~World();

protected:
  virtual void Init();
  virtual void Update();
  virtual void Render();



};

