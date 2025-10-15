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
class AudioComponent;


class Character : public Actor
{
public:
  Character(const String& modelName,
            const Vector3& positionOffset,
            const Vector3& scaleOffset = Vector3(1.0f,1.0f,1.0f));
  virtual ~Character() = default;

  virtual void Init() override;
  virtual void FixedUpdate() override;
  virtual void Update(float deltaTime) override;
  virtual void Render() override;

  void SetName(const String& newName){
    Object::SetName(newName);
  }

  SPtr<Model> m_model;

  Vector3 m_positionOffset;
  Vector3 m_scaleOffset;

  Vector<char> m_matrixData;

  SPtr<AudioComponent> m_audioComponent;
  
  String m_modelName;
};

