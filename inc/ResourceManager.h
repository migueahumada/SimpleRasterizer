#pragma once
#include "Module.h"
#include "HelperMacros.h"
#include "Object.h"

class ResourceManager : public Module<ResourceManager>
{
  ResourceManager();
  virtual ~ResourceManager() = default;

public:
  void OnStartUp() override;

  void OnShutdown() override;

};

//Vector<SPtr<Object>> m_resources;

ResourceManager& g_resourceManager();


