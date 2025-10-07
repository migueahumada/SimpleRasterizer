#pragma once
#include "HelperMacros.h"
#include "Object.h"

class Component : public Object
{
  Component() = default;
  virtual ~Component() = default;

  virtual void Init() = 0;

  virtual void Update() = 0;

};

