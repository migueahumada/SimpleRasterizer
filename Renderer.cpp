#include "Renderer.h"
#include "GraphicsAPI.h"
#include "Texture.h"
#include "World.h"
#include "Character.h"
#include "Model.h"
#include "GraphicsBuffer.h"
#include "Actor.h"
#include "Camera.h"

Renderer::Renderer(const WPtr<GraphicsAPI>& pGraphicsAPI,
									 const WPtr<Camera>& pCamera,
									 const WPtr<World>& pWorld) : 
									 m_pGraphicsAPI(pGraphicsAPI), 
									 m_pCamera(pCamera), 
									 m_pWorld(pWorld)
{
}

void Renderer::CompileShaders()
{
	if (m_pGraphicsAPI.expired())
	{
		SHOW_ERROR(L"Couldn't compile shaders :(")
		return;
	}

	auto GAPI = m_pGraphicsAPI.lock();

	auto pDefVertexShader = GAPI->CreateVertexShader(L"Gbuffer.hlsl",
		"gbuffer_vertex_main");
	if (pDefVertexShader)
	{
		m_pGBuffer_VS = pDefVertexShader;
	}


	auto pDefPixelShader = GAPI->CreatePixelShader(L"Gbuffer.hlsl",
		"gbuffer_pixel_main");
	if (pDefPixelShader)
	{
		m_pGBuffer_PS = pDefPixelShader;
	}


	auto pVS_DefLighting = GAPI->CreateVertexShader(L"Def_Lighting.hlsl", "vertex_main");
	if (pVS_DefLighting)
	{
		m_pVS_SSFS = pVS_DefLighting;
	}

	auto pPS_DefLighting = GAPI->CreatePixelShader(L"Def_Lighting.hlsl", "pixel_main");
	if (pPS_DefLighting)
	{
		m_pPS_DefLighting = pPS_DefLighting;
	}

	auto pPS_AO = GAPI->CreatePixelShader(L"Def_Lighting.hlsl","ao_main");
	if (pPS_AO)
	{
		m_pPS_AO = pPS_AO;
	}

	auto pVS_ShadowMap = GAPI->CreateVertexShader(L"Gbuffer.hlsl", "shadow_map_vertex_main");
	if (pVS_ShadowMap)
	{
		m_pVS_ShadowMap = pVS_ShadowMap;
	}

	auto pPS_ShadowMap = GAPI->CreatePixelShader(L"Gbuffer.hlsl", "shadow_map_pixel_main");
	if (pPS_ShadowMap)
	{
		m_pPS_ShadowMap = pPS_ShadowMap;
	}

}

void Renderer::InitInputLayout()
{
	if (m_pGraphicsAPI.expired())
	{
		SHOW_ERROR(L"Couldn't set input layout :(")
		return;
	}

	auto GAPI = m_pGraphicsAPI.lock();

	Vector<D3D11_INPUT_ELEMENT_DESC> inputElementDesc = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,	0,  0,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR"	  , 0, DXGI_FORMAT_R32G32B32_FLOAT,	0, 12,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL"  , 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT" , 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT   ,	0, 48,	D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	m_pInputLayout = GAPI->CreateInputLayout(inputElementDesc, m_pGBuffer_VS);
	if (!m_pInputLayout)
	{
		SHOW_ERROR(L"Couldn't create input layout for gbuffer");
		return;
	}

	m_pDefLightingInputLayout = GAPI->CreateInputLayout(inputElementDesc, m_pVS_SSFS);
	if (!m_pDefLightingInputLayout)
	{
		SHOW_ERROR(L"Couldn't create input layout for lighting");
		return;
	}
}

void Renderer::InitCamera(int width, int height)
{
	if (m_pCamera.expired())
	{
		SHOW_ERROR(L"Couldn't init camera :(")
			return;
	}

	auto CAMERA = m_pCamera.lock();

	CAMERA->SetLookAt(Vector3(0, 0, -6.0f), Vector3(0, 0, 0), Vector3(0, 1, 0));		//Setea la matriz de la cámara
	CAMERA->SetPerspective(3.141592653f / 4.0f, width, height, 0.1f, 1000.0f);
}

void Renderer::InitWVP()
{

	if (m_pCamera.expired())
	{
		SHOW_ERROR(L"Couldn't inti WVP :(")
			return;
	}

	auto CAMERA = m_pCamera.lock();

	//---------------WORLD SETUP---------------
	m_WVP.world.Identity();															//Matriz de mundo se hace identidad
	m_WVP.view = CAMERA->getViewMatrix();											//Matriz de vista se obtiene de cámara
	m_WVP.projection = CAMERA->getProjectionMatrix();								//Matriz de proyección se obtiene de cámara
	m_WVP.viewDir = CAMERA->GetViewDir();

	m_WVP.ligthView.Identity();
	m_WVP.ligthView.LookAt(Vector3(20.0f,20.0f, 5.0f),Vector3(0, 0, 0),Vector3(0, 1, 0));
	m_WVP.lightProjection.SetOrthographic(-5, 5, -5, 5, 0.1f, 100.0f);

	m_WVP.world.Transpose();														//Se transpone
	m_WVP.view.Transpose();															//Se transpone
	m_WVP.projection.Transpose();													//Se transpone
	m_WVP.ligthView.Transpose();
	m_WVP.lightProjection.Transpose();
}

void Renderer::InitConstantBuffer()
{
	if (m_pGraphicsAPI.expired())
	{
		SHOW_ERROR(L"Couldn't set constant buffer :(")
			return;
	}

	auto GAPI = m_pGraphicsAPI.lock();

	Vector<char> matrix_data;
	matrix_data.resize(sizeof(m_WVP));
	memcpy(matrix_data.data(),&m_WVP,sizeof(m_WVP));

	m_pCB_WVP = GAPI->CreateConstantBuffer(matrix_data);
	if (!m_pCB_WVP)
	{
		SHOW_ERROR(L"Couldn't constant buffer");
		return;
	}

}

void Renderer::InitRasterizerStates()
{
	if (m_pGraphicsAPI.expired())
	{
		SHOW_ERROR(L"Couldn't set rasterizer states :(")
			return;
	}

	auto GAPI = m_pGraphicsAPI.lock();

	ID3D11RasterizerState1* pRasterState = nullptr;

	m_RasterStates.insert({	RasterStates::DEFAULT,		pRasterState});
	m_RasterStates.insert({ RasterStates::WIREFRAME,	pRasterState });
	m_RasterStates.insert({ RasterStates::NO_CULLING,	pRasterState });

	CD3D11_RASTERIZER_DESC1 descRD(D3D11_DEFAULT);
	
	
	HRESULT hr;
	hr = GAPI->m_pDevice->CreateRasterizerState1(&descRD, 
																							 &m_RasterStates.at(RasterStates::DEFAULT));
	if (FAILED(hr))
	{
		SHOW_ERROR(L"Couldn't create Default Rasterizer State");
		return;
	}

	descRD = CD3D11_RASTERIZER_DESC1 (D3D11_DEFAULT);
	descRD.FillMode = D3D11_FILL_WIREFRAME;
	hr = GAPI->m_pDevice->CreateRasterizerState1(&descRD, 
																							 &m_RasterStates.at(RasterStates::WIREFRAME));
	if (FAILED(hr))
	{
		SHOW_ERROR(L"Couldn't create Wireframe Rasterizer State");
		return;
	}

	descRD = CD3D11_RASTERIZER_DESC1(D3D11_DEFAULT);
	descRD.CullMode = D3D11_CULL_NONE;
	hr = GAPI->m_pDevice->CreateRasterizerState1(&descRD, 
																						   &m_RasterStates.at(RasterStates::NO_CULLING));
	if (FAILED(hr))
	{
		SHOW_ERROR(L"Couldn't create CullNone Rasterizer State");
		return;
	}
}

void Renderer::InitSampleFilters()
{
	if (m_pGraphicsAPI.expired())
	{
		SHOW_ERROR(L"Couldn't set sampler states :(")
			return;
	}

	auto GAPI = m_pGraphicsAPI.lock();

	ID3D11SamplerState* pSamplerState = nullptr;

	m_SamplerStates.insert({ SamplerStates::POINT,			 pSamplerState});
	m_SamplerStates.insert({ SamplerStates::LINEAR,			 pSamplerState });
	m_SamplerStates.insert({ SamplerStates::ANISOTROPIC, pSamplerState });

	CD3D11_SAMPLER_DESC descSS(D3D11_DEFAULT);
	descSS.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	descSS.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;

	descSS.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	
	HRESULT hr;
	hr = GAPI->m_pDevice->CreateSamplerState(&descSS, 
																					 &m_SamplerStates.at(SamplerStates::POINT));
	if (FAILED(hr))
	{
		SHOW_ERROR(L"Couldn't create Point Sampler State");
		return;
	}

	descSS.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	hr = GAPI->m_pDevice->CreateSamplerState(&descSS, 
																					 &m_SamplerStates.at(SamplerStates::LINEAR));
	if (FAILED(hr))
	{
		SHOW_ERROR(L"Couldn't create Linear Sampler State");
		return;
	}

	descSS.Filter = D3D11_FILTER_ANISOTROPIC;
	descSS.MaxAnisotropy = 16;
	hr = GAPI->m_pDevice->CreateSamplerState(&descSS, 
																					 &m_SamplerStates.at(SamplerStates::ANISOTROPIC));
	if (FAILED(hr))
	{
		SHOW_ERROR(L"Couldn't create Anisotropic Sampler State");
		return;
	}
}

void Renderer::InitGBuffer(int width, int height)
{
	if (m_pGraphicsAPI.expired())
	{
		SHOW_ERROR(L"Couldn't init GBuffer:(")
			return;
	}

	auto GAPI = m_pGraphicsAPI.lock();

	m_GBuffer.resize(4);

	//Position texture
	m_GBuffer[0].m_pTexture = GAPI->CreateTexture(width,
																							  height,
																							  DXGI_FORMAT_R32G32B32A32_FLOAT,
																							  D3D11_USAGE_DEFAULT,
																							  D3D11_BIND_RENDER_TARGET |
																							  D3D11_BIND_SHADER_RESOURCE,
																							  0,
																							  1,
																							  &m_GBuffer[0].m_pSRV,
																							  &m_GBuffer[0].m_pRTV);
	//Color texture
	m_GBuffer[1].m_pTexture = GAPI->CreateTexture(width,
																								height,
																								DXGI_FORMAT_R8G8B8A8_UNORM,
																								D3D11_USAGE_DEFAULT,
																								D3D11_BIND_RENDER_TARGET |
																								D3D11_BIND_SHADER_RESOURCE,
																								0,
																								1,
																								&m_GBuffer[1].m_pSRV,
																								&m_GBuffer[1].m_pRTV);
	//normal texture
	m_GBuffer[2].m_pTexture = GAPI->CreateTexture(width,
																								height,
																								DXGI_FORMAT_R8G8B8A8_UNORM,
																								D3D11_USAGE_DEFAULT,
																								D3D11_BIND_RENDER_TARGET |
																								D3D11_BIND_SHADER_RESOURCE,
																								0,
																								1,
																								&m_GBuffer[2].m_pSRV,
																								&m_GBuffer[2].m_pRTV);
	//Ambient Occlusion
	m_GBuffer[3].m_pTexture = GAPI->CreateTexture(width,
																								height,
																								DXGI_FORMAT_R16_UNORM,
																								D3D11_USAGE_DEFAULT,
																								D3D11_BIND_RENDER_TARGET |
																								D3D11_BIND_SHADER_RESOURCE,
																								0,
																								1,
																								&m_GBuffer[3].m_pSRV,
																								&m_GBuffer[3].m_pRTV);

	m_dsShadowMap.m_pTexture = GAPI->CreateTexture(width,
																								 height,
																								 DXGI_FORMAT_D32_FLOAT,
																								 D3D11_USAGE_DEFAULT,
																								 D3D11_BIND_DEPTH_STENCIL | 
																								 D3D11_BIND_SHADER_RESOURCE,
																								 0,
																								 1,
																								 &m_dsShadowMap.m_pSRV,
																								 &m_dsShadowMap.m_pRTV,
																								 &m_dsShadowMap.m_pDSV,
																								 &m_dsShadowMap.m_pDSV_RO);
}

void Renderer::SetDefaultTextures()
{
	CreateDefaultSRV(0xFFFFFF,DefaultTextures::WHITE);
	CreateDefaultSRV(0x000000, DefaultTextures::BLACK);
	CreateDefaultSRV(0xFF8080, DefaultTextures::NORMAL, DXGI_FORMAT_R8G8B8A8_UNORM);
	
}

void Renderer::RenderActor(const WPtr<Character>& character)
{
	
	if (m_pGraphicsAPI.expired() || character.expired() || m_pCamera.expired())
	{
		SHOW_ERROR(L"Couldn't render Actor :(")
		return;
	}
	auto CHAR = character.lock();
	auto GAPI = m_pGraphicsAPI.lock();
	auto CAMERA = m_pCamera.lock();

	ID3D11ShaderResourceView* pNullSRV = nullptr;

	GAPI->m_pDeviceContext->IASetInputLayout(m_pInputLayout);

	GAPI->m_pDeviceContext->IASetPrimitiveTopology(
		static_cast<D3D11_PRIMITIVE_TOPOLOGY>(CHAR->m_model->m_meshes[0].topology));

	UINT stride = sizeof(MODEL_VERTEX);
	UINT offset = 0;

	GAPI->m_pDeviceContext->IASetVertexBuffers(0,1,
																							&CHAR->m_model->m_pVertexBuffer->m_pBuffer, 
																							&stride, &offset);
	GAPI->m_pDeviceContext->IASetIndexBuffer(CHAR->m_model->m_pIndexBuffer->m_pBuffer,
																						DXGI_FORMAT_R16_UINT,0);

	GAPI->m_pDeviceContext->PSSetSamplers(0, 1, &m_SamplerStates.at(SamplerStates::POINT));
	GAPI->m_pDeviceContext->PSSetSamplers(1, 1, &m_SamplerStates.at(SamplerStates::LINEAR));
	GAPI->m_pDeviceContext->PSSetSamplers(2, 1, &m_SamplerStates.at(SamplerStates::ANISOTROPIC));
	

	m_WVP.world = CHAR->getLocalTransform().getMatrix();
	m_WVP.view = CAMERA->getViewMatrix();
	m_WVP.projection = CAMERA->getProjectionMatrix();
	m_WVP.viewDir = CAMERA->GetViewDir();

	//GPASS
	m_WVP.projection.Transpose();
	m_WVP.view.Transpose();
	m_WVP.world.Transpose();

	Vector<char> matrix_data;
	matrix_data.resize(sizeof(m_WVP));
	memcpy(matrix_data.data(), &m_WVP, sizeof(m_WVP));

	GAPI->m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pCB_WVP->m_pBuffer);
	GAPI->m_pDeviceContext->PSSetConstantBuffers(0, 1, &m_pCB_WVP->m_pBuffer);

	GAPI->m_pDeviceContext->RSSetState(m_RasterStates.at(RasterStates::DEFAULT));
	GAPI->writeToBuffer(m_pCB_WVP, matrix_data);

	

	GAPI->m_pDeviceContext->PSSetShaderResources(0, 1, &CHAR->m_texture->m_pSRV);

	if (!CHAR->m_normalTextureName.empty())
	{
		GAPI->m_pDeviceContext->PSSetShaderResources(1, 1, &CHAR->m_normalTexture->m_pSRV);
	}
	else
		GAPI->m_pDeviceContext->PSSetShaderResources(1, 1, &m_DefaultTextures.at(DefaultTextures::NORMAL));

	if (!CHAR->m_roughnessTextureName.empty())
	{
		GAPI->m_pDeviceContext->PSSetShaderResources(2, 1, &CHAR->m_roughnessTexture->m_pSRV);
	}
	else
		GAPI->m_pDeviceContext->PSSetShaderResources(2, 1, &m_DefaultTextures.at(DefaultTextures::BLACK));

	if (!CHAR->m_metallicTextureName.empty())
	{
		GAPI->m_pDeviceContext->PSSetShaderResources(3, 1, &CHAR->m_metallicTexture->m_pSRV);
	}
	else
		GAPI->m_pDeviceContext->PSSetShaderResources(3, 1, &m_DefaultTextures.at(DefaultTextures::WHITE));

	GAPI->m_pDeviceContext->DrawIndexed(CHAR->m_model->m_meshes[0].numIndices,
																			CHAR->m_model->m_meshes[0].baseIndex,
																			CHAR->m_model->m_meshes[0].baseVertex);
	
	
	

}

void Renderer::RenderShadows(const WPtr<Character>& character)
{
	if (m_pGraphicsAPI.expired() || character.expired() || m_pCamera.expired())
	{
		SHOW_ERROR(L"Couldn't render Actor :(")
			return;
	}
	auto CHAR = character.lock();
	auto GAPI = m_pGraphicsAPI.lock();
	auto CAMERA = m_pCamera.lock();

	GAPI->m_pDeviceContext->IASetInputLayout(m_pInputLayout);

	GAPI->m_pDeviceContext->IASetPrimitiveTopology(
		static_cast<D3D11_PRIMITIVE_TOPOLOGY>(CHAR->m_model->m_meshes[0].topology));

	UINT stride = sizeof(MODEL_VERTEX);
	UINT offset = 0;

	GAPI->m_pDeviceContext->IASetVertexBuffers(0, 1,
																						 &CHAR->m_model->m_pVertexBuffer->m_pBuffer,
																						 &stride, &offset);
	GAPI->m_pDeviceContext->IASetIndexBuffer(CHAR->m_model->m_pIndexBuffer->m_pBuffer,
																					 DXGI_FORMAT_R16_UINT, 0);

	m_WVP.world = CHAR->getLocalTransform().getMatrix();
	m_WVP.view = CAMERA->getViewMatrix();
	m_WVP.projection = CAMERA->getProjectionMatrix();
	m_WVP.viewDir = CAMERA->GetViewDir();

	////GPASS
	m_WVP.projection.Transpose();
	m_WVP.view.Transpose();
	m_WVP.world.Transpose();

	Vector<char> matrix_data;
	matrix_data.resize(sizeof(m_WVP));
	memcpy(matrix_data.data(), &m_WVP, sizeof(m_WVP));

	GAPI->m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pCB_WVP->m_pBuffer);
	GAPI->m_pDeviceContext->PSSetConstantBuffers(0, 1, &m_pCB_WVP->m_pBuffer);

	GAPI->m_pDeviceContext->RSSetState(m_RasterStates.at(RasterStates::DEFAULT));
	GAPI->writeToBuffer(m_pCB_WVP, matrix_data);

	GAPI->m_pDeviceContext->DrawIndexed(CHAR->m_model->m_meshes[0].numIndices,
																			CHAR->m_model->m_meshes[0].baseIndex,
																			CHAR->m_model->m_meshes[0].baseVertex);
}

void Renderer::CreateDefaultSRV(UINT value, DefaultTextures::E defaultTextureType, DXGI_FORMAT format)
{
	if (m_pGraphicsAPI.expired())
	{
		SHOW_ERROR(L"Couldn't set Default Textures :(")
			return;
	}

	auto GAPI = m_pGraphicsAPI.lock();

	HRESULT hr;

	//White
	D3D11_TEXTURE2D_DESC desc = { 0 };
	desc.Width = 1;
	desc.Height = 1;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = format;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.SampleDesc.Count = 1;

	UINT pixel = value;
	D3D11_SUBRESOURCE_DATA initData = { 0 };
	initData.pSysMem = &pixel;
	initData.SysMemPitch = 4;

	ID3D11Texture2D* tex = nullptr;

	hr = GAPI->m_pDevice->CreateTexture2D(&desc, &initData, &tex);

	if (FAILED(hr))
	{
		SHOW_ERROR(L"Failed to create white texture.");
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = CD3D11_SHADER_RESOURCE_VIEW_DESC();
	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	ID3D11ShaderResourceView* pSRV = nullptr;

	hr = GAPI->m_pDevice->CreateShaderResourceView(tex, &srvDesc, &pSRV);
	if (FAILED(hr))
	{
		tex->Release();
		SHOW_ERROR(L"srv for texture couldn't be created.");
		return;
	}

	m_DefaultTextures.insert({ defaultTextureType, pSRV });
}

void Renderer::SetShadowPass()
{
	if (m_pGraphicsAPI.expired() || m_pWorld.expired())
	{
		SHOW_ERROR(L"Couldn't set Shadow Pass :(")
			return;
	}

	auto GAPI = m_pGraphicsAPI.lock();
	auto WORLD = m_pWorld.lock();

	GAPI->m_pDeviceContext->ClearDepthStencilView(m_dsShadowMap.m_pDSV,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f, 0);

	const Vector<ID3D11RenderTargetView*> renderTargets =
	{
		nullptr,
		nullptr,
		nullptr
	};

	GAPI->m_pDeviceContext->VSSetShader(m_pVS_ShadowMap->m_pVertexShader,
		nullptr,
		0);
	GAPI->m_pDeviceContext->PSSetShader(m_pPS_ShadowMap->m_pPixelShader,
		nullptr,
		0);
	GAPI->m_pDeviceContext->OMSetRenderTargets(renderTargets.size(),
																						 renderTargets.data(),
																						 m_dsShadowMap.m_pDSV);

	for (auto& actor : WORLD->getActors())
	{
		auto character = std::dynamic_pointer_cast<Character>(actor);

		if (character)
		{
			RenderShadows(character);
		}
	}
	
}

void Renderer::SetGeometryPass()
{
	if (m_pGraphicsAPI.expired() || m_pWorld.expired())
	{
		SHOW_ERROR(L"Couldn't set Geometry Pass :(")
			return;
	}

	auto GAPI = m_pGraphicsAPI.lock();
	auto WORLD = m_pWorld.lock();
	
	FloatColor tempColor = Color{ 0, 0, 0, 0 };
	float clearColor2[4] = { tempColor.r,tempColor.g , tempColor.b,tempColor.a };

	GAPI->m_pDeviceContext->ClearRenderTargetView(GAPI->m_pBackBufferRTV,
																								clearColor2);
	GAPI->m_pDeviceContext->ClearRenderTargetView(m_GBuffer[0].m_pRTV,
																								clearColor2);
	GAPI->m_pDeviceContext->ClearRenderTargetView(m_GBuffer[1].m_pRTV,
																								clearColor2);
	GAPI->m_pDeviceContext->ClearRenderTargetView(m_GBuffer[2].m_pRTV,
																								clearColor2);
	GAPI->m_pDeviceContext->ClearRenderTargetView(m_GBuffer[3].m_pRTV,
																								clearColor2);

	GAPI->m_pDeviceContext->ClearDepthStencilView(GAPI->m_pBackBufferDSV,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f, 0);

	const Vector<ID3D11RenderTargetView*> renderTargets =
	{
		m_GBuffer[0].m_pRTV,
		m_GBuffer[1].m_pRTV,
		m_GBuffer[2].m_pRTV
	};

	GAPI->m_pDeviceContext->OMSetRenderTargets(renderTargets.size(),
																						 renderTargets.data(),
																						 GAPI->m_pBackBufferDSV);

	GAPI->m_pDeviceContext->VSSetShader(m_pGBuffer_VS->m_pVertexShader,
																			nullptr,
																			0);
	GAPI->m_pDeviceContext->PSSetShader(m_pGBuffer_PS->m_pPixelShader,
																			nullptr,
																			0);

	for (const auto& actor : WORLD->getActors())
	{
		auto character = std::dynamic_pointer_cast<Character>(actor);

		if (character)
		{
			RenderActor(character);
			//SetShadowPass(character);
			
			
		}
	}

}

void Renderer::SetSSAOPass()
{
	if (m_pGraphicsAPI.expired())
	{
		SHOW_ERROR(L"Couldn't set Geometry Pass :(")
			return;
	}

	auto GAPI = m_pGraphicsAPI.lock();

	ID3D11ShaderResourceView* pNullSRV = nullptr;

	GAPI->m_pDeviceContext->VSSetShader(m_pVS_SSFS->m_pVertexShader,
																			nullptr,
																			0);
	GAPI->m_pDeviceContext->PSSetShader(m_pPS_AO->m_pPixelShader,
																			nullptr,
																			0);

	const Vector<ID3D11RenderTargetView*> renderTargets =
	{
		m_GBuffer[3].m_pRTV,
		nullptr,
		nullptr
	};
	
	GAPI->m_pDeviceContext->OMSetRenderTargets(renderTargets.size(),
																						 renderTargets.data(),
																						 nullptr);
	
	GAPI->m_pDeviceContext->RSSetState(m_RasterStates.at(RasterStates::NO_CULLING));

	GAPI->m_pDeviceContext->PSSetShaderResources(0, 1, &m_GBuffer[0].m_pSRV);
	GAPI->m_pDeviceContext->PSSetShaderResources(1, 1, &m_GBuffer[1].m_pSRV);
	GAPI->m_pDeviceContext->PSSetShaderResources(2, 1, &m_GBuffer[2].m_pSRV);
	GAPI->m_pDeviceContext->PSSetShaderResources(3, 1, &pNullSRV);

	GAPI->m_pDeviceContext->IASetInputLayout(m_pDefLightingInputLayout);
	
	GAPI->m_pDeviceContext->Draw(3, 0);

	GAPI->m_pDeviceContext->PSSetShaderResources(0, 1, &pNullSRV);
	GAPI->m_pDeviceContext->PSSetShaderResources(1, 1, &pNullSRV);
	GAPI->m_pDeviceContext->PSSetShaderResources(2, 1, &pNullSRV);
	GAPI->m_pDeviceContext->PSSetShaderResources(3, 1, &pNullSRV);

}

void Renderer::SetLightingPass()
{
	
	if (m_pGraphicsAPI.expired())
	{
		SHOW_ERROR(L"Couldn't set Geometry Pass :(")
			return;
	}

	auto GAPI = m_pGraphicsAPI.lock();

	ID3D11ShaderResourceView* pNullSRV = nullptr;

	GAPI->m_pDeviceContext->VSSetShader(m_pVS_SSFS->m_pVertexShader,
		nullptr,
		0);
	GAPI->m_pDeviceContext->PSSetShader(m_pPS_DefLighting->m_pPixelShader,
		nullptr,
		0);

	const Vector<ID3D11RenderTargetView*> renderTargets =
	{
		GAPI->m_pBackBufferRTV,
		nullptr,
		nullptr
	};

	GAPI->m_pDeviceContext->OMSetRenderTargets(renderTargets.size(),
																							renderTargets.data(),
																							nullptr);

	GAPI->m_pDeviceContext->RSSetState(m_RasterStates.at(RasterStates::NO_CULLING));

	GAPI->m_pDeviceContext->PSSetShaderResources(0, 1, &m_GBuffer[0].m_pSRV);
	GAPI->m_pDeviceContext->PSSetShaderResources(1, 1, &m_GBuffer[1].m_pSRV);
	GAPI->m_pDeviceContext->PSSetShaderResources(2, 1, &m_GBuffer[2].m_pSRV);
	GAPI->m_pDeviceContext->PSSetShaderResources(3, 1, &m_GBuffer[3].m_pSRV);
	GAPI->m_pDeviceContext->PSSetShaderResources(4, 1, &m_dsShadowMap.m_pSRV);

	GAPI->m_pDeviceContext->IASetInputLayout(m_pDefLightingInputLayout);
	GAPI->m_pDeviceContext->Draw(3, 0);//DRAWWW

	GAPI->m_pDeviceContext->PSSetShaderResources(0, 1, &pNullSRV);
	GAPI->m_pDeviceContext->PSSetShaderResources(1, 1, &pNullSRV);
	GAPI->m_pDeviceContext->PSSetShaderResources(2, 1, &pNullSRV);
	GAPI->m_pDeviceContext->PSSetShaderResources(3, 1, &pNullSRV);
	GAPI->m_pDeviceContext->PSSetShaderResources(4, 1, &pNullSRV);

}

