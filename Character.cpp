#include "Character.h"
#include "GraphicsAPI.h"
#include "Model.h"
#include "Image.h"
#include "Texture.h"
#include "Camera.h"

Character::Character(GraphicsAPI* pGraphicsAPI, MatrixCollection* WVP, Camera* camera) 
  : m_pGraphicsAPI(pGraphicsAPI), m_pWVP(WVP), m_pCamera(camera)
{

  m_model = std::make_unique<Model>();
  m_model->LoadFromFile(m_modelName.c_str(), *m_pGraphicsAPI);
  
  m_image = std::make_unique<Image>();
  m_image->decode("Rex_C.bmp");

  m_texture = std::make_unique<Texture>();
  m_texture->createImage(*m_image.get(), *m_pGraphicsAPI);

}

void Character::Init()
{
  
}

void Character::Update(float deltaTime)
{
}

void Character::Render()
{
  UINT stride = sizeof(MODEL_VERTEX);
  UINT offset = 0;

  m_pGraphicsAPI->m_pDeviceContext->IASetPrimitiveTopology(
    static_cast<D3D11_PRIMITIVE_TOPOLOGY>(m_model->m_meshes[0].topology));
  
  m_pGraphicsAPI->m_pDeviceContext->IASetVertexBuffers(
    0,1,&m_model->m_pVertexBuffer->m_pBuffer,&stride,&offset);

  m_pGraphicsAPI->m_pDeviceContext->IASetIndexBuffer(
    m_model->m_pIndexBuffer->m_pBuffer, DXGI_FORMAT_R16_UINT, 0);
  m_pGraphicsAPI->m_pDeviceContext->PSSetShaderResources(0,1,&m_texture->m_pSRV);

  m_transform.Identity();
  m_transform.Translate(Vector3(0.0f, 0.0f, 6.0f));

  m_pWVP->world = m_transform;
  m_pWVP->projection = m_pCamera->getProjectionMatrix();

  m_pWVP->world.Transpose();
  m_pWVP->projection.Transpose();
  
  m_matrixData.resize(sizeof(MatrixCollection));
  
  memcpy(m_matrixData.data(), &*m_pWVP, sizeof(MatrixCollection));

  //TODO: constant buffers need to be created once and updated every frame
  //g_pCB_WVP = g_pGraphicsAPI->CreateConstantBuffer(matrix_data);

  /* 
  
  Matrix4 translation3;
	translation3.Identity();
	translation3.Translate(Vector3(6.0f, 0.0f, 0.0f));

	g_WVP.world = translation3;
	g_WVP.projection = g_Camera.getProjectionMatrix();

	g_WVP.world.Transpose();
	g_WVP.projection.Transpose();

	memcpy(matrix_data.data(), &g_WVP, sizeof(g_WVP));

	g_pGraphicsAPI->m_pDeviceContext->VSSetConstantBuffers(0, 1, &g_pCB_WVP->m_pBuffer);

	g_pGraphicsAPI->writeToBuffer(g_pCB_WVP, matrix_data);

	g_pGraphicsAPI->m_pDeviceContext->DrawIndexed(g_AmbulanceModel.m_meshes[0].numIndices, g_AmbulanceModel.m_meshes[0].baseIndex, g_AmbulanceModel.m_meshes[0].baseVertex);

  */
  

}
