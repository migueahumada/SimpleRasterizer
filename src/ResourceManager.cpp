#include "ResourceManager.h"
#include "GraphicsAPI.h"

void ResourceManager::OnStartUp()
{
}

void ResourceManager::OnShutdown()
{
}

ResourceManager& g_resourceManager()
{
  return ResourceManager::GetInstance();
}
