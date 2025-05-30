#include "Character.h"
#include "GraphicsAPI.h"
#include "Model.h"
#include "Image.h"
#include "Texture.h"
#include "Camera.h"

Character::Character( WPtr<GraphicsAPI> pGraphicsAPI,
                      MatrixCollection& WVP,
                      WPtr<Camera> camera,
                      WPtr<GraphicsBuffer> constBuffer,
                      const String& modelName,
                      const String& textureName,
                      const Vector3& positionOffset,
                      const String& normalTextureName,
                      const String& roughnessTextureName,
                      const String& metallicTextureName):
                      m_pGraphicsAPI(pGraphicsAPI), 
                      m_WVP(WVP), 
                      m_pCamera(camera),
                      m_pCB(constBuffer),
                      m_modelName(modelName),
                      m_textureName(textureName),
                      m_positionOffset(positionOffset),
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
  m_localTransform.setRotation(Vector3(0.0f, 230.0f,0.0f));

}

void Character::Update(float deltaTime)
{
  Actor::Update(deltaTime);
   
}

void Character::Render()
{
  UINT stride = sizeof(MODEL_VERTEX);
  UINT offset = 0;

  auto GAPI = m_pGraphicsAPI.lock();
  auto camera = m_pCamera.lock();
  auto CB = m_pCB.lock();

  if (!GAPI || !camera || !CB)
  {
    return;
  }

  GAPI->m_pDeviceContext->IASetPrimitiveTopology(
    static_cast<D3D11_PRIMITIVE_TOPOLOGY>(m_model->m_meshes[0].topology));
  
  GAPI->m_pDeviceContext->IASetVertexBuffers(
    0,1,&m_model->m_pVertexBuffer->m_pBuffer,&stride,&offset);

  GAPI->m_pDeviceContext->IASetIndexBuffer(
    m_model->m_pIndexBuffer->m_pBuffer, DXGI_FORMAT_R16_UINT, 0);
  GAPI->m_pDeviceContext->PSSetShaderResources(0,1, &m_texture->m_pSRV);
  
  if (!m_normalTextureName.empty())
  {
    GAPI->m_pDeviceContext->PSSetShaderResources(1, 1, &m_normalTexture->m_pSRV);
  }

  if (!m_roughnessTextureName.empty())
  {
    GAPI->m_pDeviceContext->PSSetShaderResources(2, 1, &m_roughnessTexture->m_pSRV);
  }

  if (!m_metallicTextureName.empty())
  {
    GAPI->m_pDeviceContext->PSSetShaderResources(3, 1, &m_metallicTexture->m_pSRV);
  }

  
  m_WVP.world = m_localTransform.getMatrix();
  m_WVP.view = camera->getViewMatrix();
  m_WVP.projection = camera->getProjectionMatrix();
  m_WVP.viewDir = camera->GetViewDir();

  m_WVP.projection.Transpose();
  m_WVP.view.Transpose();
  m_WVP.world.Transpose();
  

  // TODO: Renderer tendría que obtener la info del character para mandarlo al shader.
  // Renderer tendría que tener el Constant Buffer
  Vector<char> matrix_data;
  matrix_data.resize(sizeof(m_WVP));
  memcpy(matrix_data.data(), &m_WVP, sizeof(m_WVP));

  GAPI->m_pDeviceContext->VSSetConstantBuffers(0, 1, &CB->m_pBuffer);
  GAPI->m_pDeviceContext->PSSetConstantBuffers(0, 1, &CB->m_pBuffer);

  GAPI->writeToBuffer(CB, matrix_data);

  GAPI->m_pDeviceContext->DrawIndexed(m_model->m_meshes[0].numIndices, 
                                      m_model->m_meshes[0].baseIndex, 
                                      m_model->m_meshes[0].baseVertex);

}


