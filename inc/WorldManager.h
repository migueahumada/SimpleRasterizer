#pragma once
#include "Module.h"
#include "HelperMacros.h"

class WorldManager : public Module<WorldManager>
{
public:
  WorldManager() = default;
  virtual ~WorldManager() = default;

  void OnStartUp() override;
  void OnShutdown() override;

private:
  
  
};

