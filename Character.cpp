#include "Character.h"
#include "GraphicsAPI.h"
#include "Model.h"
#include "Image.h"
#include "Texture.h"
#include "Camera.h"

Character::Character( WPtr<GraphicsAPI> pGraphicsAPI,
                      MatrixCollection& WVP,
                      WPtr<Camera> camera,
                      WPtr<GraphicsBuffer> constBuffer) :
                      m_pGraphicsAPI(pGraphicsAPI), 
                      m_WVP(WVP), 
                      m_pCamera(camera),
                      m_pCB(constBuffer)

{

  m_model = std::make_shared<Model>();
  m_image = std::make_shared<Image>();
  m_texture = std::make_shared<Texture>();
  
}

void Character::Init()
{
  m_image->decode(m_textureName.c_str());
  m_model->LoadFromFile(m_modelName.c_str(), m_pGraphicsAPI);
  m_texture->createImage(*m_image, m_pGraphicsAPI);

  m_transform.Identity();
  m_transform.Translate(Vector3(0.0f, 0.0f, 6.0f));
  m_transform.RotateY(3.3f);
  
}

void Character::Update(float deltaTime)
{
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

  //TODO: no hardcodear el como seteamos el mesh
  GAPI->m_pDeviceContext->IASetPrimitiveTopology(
    static_cast<D3D11_PRIMITIVE_TOPOLOGY>(m_model->m_meshes[0].topology));
  
  GAPI->m_pDeviceContext->IASetVertexBuffers(
    0,1,&m_model->m_pVertexBuffer->m_pBuffer,&stride,&offset);

  GAPI->m_pDeviceContext->IASetIndexBuffer(
    m_model->m_pIndexBuffer->m_pBuffer, DXGI_FORMAT_R16_UINT, 0);
  GAPI->m_pDeviceContext->PSSetShaderResources(0,1,&m_texture->m_pSRV);

  m_WVP.world = m_transform;
  m_WVP.view = camera->getViewMatrix();
  m_WVP.projection = camera->getProjectionMatrix();

  m_WVP.world.Transpose();
  m_WVP.view.Transpose();
  m_WVP.projection.Transpose();
  
  Vector<char> matrix_data;
  matrix_data.resize(sizeof(MatrixCollection));
  memcpy(matrix_data.data(), &m_WVP, sizeof(MatrixCollection));

  GAPI->m_pDeviceContext->VSSetConstantBuffers(0, 1, &CB->m_pBuffer);

  GAPI->writeToBuffer(CB, matrix_data);

  GAPI->m_pDeviceContext->DrawIndexed(m_model->m_meshes[0].numIndices, 
                                      m_model->m_meshes[0].baseIndex, 
                                      m_model->m_meshes[0].baseVertex);

}
