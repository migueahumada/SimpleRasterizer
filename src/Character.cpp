#include "Character.h"
#include "GraphicsAPI.h"
#include "Model.h"
#include "Image.h"
#include "Texture.h"
#include "Camera.h"
#include "GraphicsBuffer.h"

Character::Character( const WPtr<GraphicsAPI>& pGraphicsAPI,
                      const String& modelName,
                      const String& textureName,
                      const Vector3& positionOffset,
                      const Vector3& scaleOffset,
                      const String& normalTextureName,
                      const String& roughnessTextureName,
                      const String& metallicTextureName) :
                      m_pGraphicsAPI(pGraphicsAPI),
                      m_modelName(modelName),
                      m_textureName(textureName),
                      m_positionOffset(positionOffset),
                      m_scaleOffset(scaleOffset),
                      m_normalTextureName(normalTextureName),
                      m_roughnessTextureName(roughnessTextureName),
                      m_metallicTextureName(metallicTextureName)

{

  m_model = std::make_shared<Model>();
  
  m_image = std::make_shared<Image>();
  m_texture = std::make_shared<Texture>();
  
  m_normalImage = std::make_shared<Image>();
  m_normalTexture = std::make_shared<Texture>();

  m_roughnessImage = std::make_shared<Image>();
  m_roughnessTexture = std::make_shared<Texture>();

  m_metallicImage = std::make_shared<Image>();
  m_metallicTexture = std::make_shared<Texture>();
}

void Character::Init()
{
  Actor::Init();
  m_model->LoadFromFile(m_modelName.c_str(), m_pGraphicsAPI);
  
  m_image->decode(m_textureName.c_str());
  m_texture->createImage(*m_image, m_pGraphicsAPI);
  
  
  if (!m_normalTextureName.empty())
  {
    m_normalImage->decode(m_normalTextureName.c_str());
    m_normalTexture->createImage(*m_normalImage, m_pGraphicsAPI);
  }

  if (!m_roughnessTextureName.empty())
  {
    m_roughnessImage->decode(m_roughnessTextureName.c_str());
    m_roughnessTexture->createImage(*m_roughnessImage, m_pGraphicsAPI);
  }

  if (!m_metallicTextureName.empty())
  {
    m_metallicImage->decode(m_metallicTextureName.c_str());
    m_metallicTexture->createImage(*m_metallicImage, m_pGraphicsAPI);
  }
  

  m_localTransform.setPosition(m_positionOffset);
  m_localTransform.setRotation(Vector3(0.0f, 180.0f,0.0f));
  m_localTransform.setScale(m_scaleOffset);

}

void Character::Update(float deltaTime)
{
  Actor::Update(deltaTime);
   
}

void Character::Render()
{
  
}


