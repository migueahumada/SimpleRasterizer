#pragma once
#include "Actor.h"
#include "HelperMacros.h"


class Model;
class Image;
class Texture;
class GraphicsAPI;
class Camera;
struct MatrixCollection;


class Character : public Actor
{
public:
  Character(GraphicsAPI* pGraphicsAPI, MatrixCollection* WVP, Camera* camera);
  virtual ~Character() = default;

  virtual void Init() override;
  virtual void Update(float deltaTime) override;
  virtual void Render() override;

protected:
  UPtr<Model> m_model;
  UPtr<Image> m_image;
  UPtr<Texture> m_texture;
  //UPtr<GraphicsBuffer> g_pCB_WVP;

  Vector<char> m_matrixData;

  GraphicsAPI* m_pGraphicsAPI = nullptr;
  MatrixCollection* m_pWVP = nullptr;
  Camera* m_pCamera = nullptr;

  Matrix4 m_transform;


  std::string m_modelName = "yoyoModel.obj";
};

