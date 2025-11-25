#pragma once
#include "Module.h"
#include "HelperMacros.h"
#include "Object.h"

class ResourceManager : public Module<ResourceManager>
{
 public:
  ResourceManager() = default;
  virtual ~ResourceManager() = default;

 public:
  void OnStartUp() override;

  void OnShutdown() override;
 
 private:
  friend class MKEngine::UUID;
  Map<MKEngine::UUID, String> m_appUUID;

};


//Vector<SPtr<Object>> m_resources;

ResourceManager& g_resourceManager();


