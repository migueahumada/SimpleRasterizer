#include "Light.h"

#include "GraphicsAPI.h"
#include "GraphicsBuffer.h"

Light::Light(const SPtr<GraphicsAPI>& pGraphicsAPI) : m_pGraphicsAPI(pGraphicsAPI)
{
  SetConstantBuffer();
}

Light::~Light()
{
  
}

void Light::Update(float deltaTime)
{

}

void Light::Render()
{

}

void Light::SetConstantBuffer()
{
  if (m_pGraphicsAPI.expired())
  {
    return;
  }

  auto GAPI = m_pGraphicsAPI.lock();

  Vector<char> data;
  data.resize(sizeof(LightInfo));
  //m_pConstantBuffer = GAPI->CreateConstantBuffer(,);
}
