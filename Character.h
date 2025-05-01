#pragma once
#include "Actor.h"
#include "HelperMacros.h"


class Model;
class Image;
class Texture;
class GraphicsAPI;
class Camera;
struct MatrixCollection;
class GraphicsBuffer;


class Character : public Actor
{
public:
  Character(WPtr<GraphicsAPI> pGraphicsAPI, 
            MatrixCollection& WVP, 
            WPtr<Camera> camera,
            WPtr<GraphicsBuffer> constBuffer);
  virtual ~Character() = default;

  virtual void Init() override;
  virtual void Update(float deltaTime) override;
  virtual void Render() override;

protected:
  SPtr<Model> m_model;
  SPtr<Image> m_image;
  SPtr<Texture> m_texture;

  WPtr<GraphicsAPI> m_pGraphicsAPI;
  MatrixCollection& m_WVP;
  WPtr<Camera> m_pCamera;
  WPtr<GraphicsBuffer> m_pCB;

  Matrix4 m_transform;

  Vector<char> m_matrixData;

  std::string m_modelName = "ManModel.obj";
  std::string m_textureName = "manText.bmp";
};

