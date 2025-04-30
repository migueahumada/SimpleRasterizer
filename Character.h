#pragma once
#include "Actor.h"
#include "HelperMacros.h"
#include "GraphicsAPI.h"

class Model;
class Image;
class Texture;

class Character : public Actor
{
public:
  explicit Character(const UPtr<GraphicsAPI>& graphicsAPI);
  virtual ~Character() = default;

  virtual void Init() override;
  virtual void Update(float deltaTime) override;
  virtual void Render() override;

protected:
  UPtr<Model> m_model;
  UPtr<Image> m_image;
  UPtr<Texture> m_texture;


  std::string m_modelName = "yoyoModel.obj";
};

