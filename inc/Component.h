#pragma once
#include "HelperMacros.h"
#include "Object.h"

class Component : public Object
{
 public:
  Component() = default;
  virtual ~Component() = default;

  virtual void Init() = 0;

  virtual void Update() = 0;

};

