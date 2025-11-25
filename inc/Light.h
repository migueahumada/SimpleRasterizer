#pragma once
#include "Actor.h"
#include "MathObjects.h"

class GraphicsBuffer;
class GraphicsAPI;

//Needs to be a divisible between 4
struct LightInfo {
  Vector3 position;
  bool isVisible;
};

class Light : public Actor
{

public:
  explicit Light(const SPtr<GraphicsAPI>& pGraphicsAPI);
  virtual ~Light();

  void Update(float deltaTime) override;
  void Render() override;

private:
  void SetConstantBuffer();

protected:

  SPtr<GraphicsBuffer> m_pConstantBuffer;
  WPtr<GraphicsAPI> m_pGraphicsAPI;

};

