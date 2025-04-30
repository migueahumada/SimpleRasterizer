#include "Character.h"
#include "Model.h"
#include "Image.h"
#include "Texture.h"

Character::Character(const UPtr<GraphicsAPI>& pGraphicsAPI)
{

  m_model = std::make_unique<Model>();
  m_model->LoadFromFile(m_modelName.c_str(), pGraphicsAPI);
  
  m_image = std::make_unique<Image>();
  m_image->decode("Rex_C.bmp");

  m_texture = std::make_unique<Texture>();
  m_texture->createImage(*m_image.get(), pGraphicsAPI);

  int i = 2;
}

void Character::Init()
{
  
}

void Character::Update(float deltaTime)
{
}

void Character::Render()
{
}
