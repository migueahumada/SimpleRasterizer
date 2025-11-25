#include "Character.h"
#include "GraphicsAPI.h"
#include "Model.h"
#include "Image.h"
#include "Texture.h"
#include "Camera.h"
#include "GraphicsBuffer.h"
#include "AudioComponent.h"

Character::Character( const String& modelName,
                      const Vector3& positionOffset,
                      const Vector3& scaleOffset) :
                      m_modelName(modelName),
                      m_positionOffset(positionOffset),
                      m_scaleOffset(scaleOffset)

{

  m_model = std::make_shared<Model>();
  
}

void Character::Init()
{
  Actor::Init();
  m_model->LoadWithAssimp(m_modelName.c_str());

  m_localTransform.setPosition(m_positionOffset);
  m_localTransform.setRotation(Vector3(0.0f, 180.0f,0.0f));
  m_localTransform.setScale(m_scaleOffset);

  m_audioComponent = make_unique<AudioComponent>("Prueba1","D:/Coding/C++/SimpleRasterizer/audio/MX_Menu_Loop.wav");

  m_audioComponent->Play();

}

void Character::FixedUpdate()
{
  //Actor::FixedUpdate();
}

void Character::Update(float deltaTime)
{
  Actor::Update(deltaTime);
   
}

void Character::Render()
{
  
}


