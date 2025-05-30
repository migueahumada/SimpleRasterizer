#pragma once
#include "Actor.h"
#include "HelperMacros.h"
#include "MathObjects.h"


class Model;
class Image;
class Texture;
class GraphicsAPI;
class Camera;
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
            const Vector3& positionOffset, 
            const String& normalTextureName = "",
            const String& roughnessTextureName = "",
            const String& metallicTextureName = "");
  virtual ~Character() = default;

  virtual void Init() override;
  virtual void Update(float deltaTime) override;
  virtual void Render() override;
private:
 
protected:
  SPtr<Model> m_model;

  SPtr<Image> m_image;
  SPtr<Texture> m_texture;

  SPtr<Image> m_normalImage;
  SPtr<Texture> m_normalTexture;

  SPtr<Image> m_roughnessImage;
  SPtr<Texture> m_roughnessTexture;

  SPtr<Image> m_metallicImage;
  SPtr<Texture> m_metallicTexture;


  WPtr<GraphicsAPI> m_pGraphicsAPI;
  MatrixCollection& m_WVP;
  WPtr<Camera> m_pCamera;
  WPtr<GraphicsBuffer> m_pCB;

  

  Vector3 m_positionOffset;

  Vector<char> m_matrixData;

  String m_modelName;
  String m_textureName;
  String m_normalTextureName;
  String m_roughnessTextureName;
  String m_metallicTextureName;
};

