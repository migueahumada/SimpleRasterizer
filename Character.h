#pragma once
#include "Actor.h"
#include "HelperMacros.h"
#include "MathObjects.h"


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
            WPtr<GraphicsBuffer> constBuffer,
            const String& modelName,
            const String& textureName,
            const Vector3& positionOffset);
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

  Vector3 m_positionOffset;

  Transform m_transform;

  Vector<char> m_matrixData;

  String m_modelName;
  String m_textureName;
};

