#include "Renderer.h"
#include "GraphicsAPI.h"
#include "Texture.h"
#include "World.h"
#include "Character.h"
#include "Model.h"
#include "GraphicsBuffer.h"
#include "Actor.h"
#include "Camera.h"
#include "GraphicsAPI.h"

Renderer::Renderer(const WPtr<Camera>& pCamera,
									 const WPtr<World>& pWorld) : 
									 m_pCamera(pCamera), 
									 m_pWorld(pWorld)
{
}

void Renderer::OnStartUp()
{
}

void Renderer::OnShutdown()
{
}

void Renderer::CompileShaders()
{

	auto pDefVertexShader = g_graphicsAPI().CreateVertexShader(L"./shaders/Gbuffer.hlsl",
		"gbuffer_vertex_main");
	if (pDefVertexShader)
	{
		m_pGBuffer_VS = pDefVertexShader;
	}


	auto pDefPixelShader = g_graphicsAPI().CreatePixelShader(L"./shaders/Gbuffer.hlsl",
		"gbuffer_pixel_main");
	if (pDefPixelShader)
	{
		m_pGBuffer_PS = pDefPixelShader;
	}


	auto pVS_DefLighting = g_graphicsAPI().CreateVertexShader(L"./shaders/Def_Lighting.hlsl", "vertex_main");
	if (pVS_DefLighting)
	{
		m_pVS_SSFS = pVS_DefLighting;
	}

	auto pPS_DefLighting = g_graphicsAPI().CreatePixelShader(L"./shaders/Def_Lighting.hlsl", "pixel_main");
	if (pPS_DefLighting)
	{
		m_pPS_DefLighting = pPS_DefLighting;
	}

	auto pPS_AO = g_graphicsAPI().CreatePixelShader(L"./shaders/Def_Lighting.hlsl","ao_main");
	if (pPS_AO)
	{
		m_pPS_AO = pPS_AO;
	}

	auto pVS_ShadowMap = g_graphicsAPI().CreateVertexShader(L"./shaders/Gbuffer.hlsl", "shadow_map_vertex_main");
	if (pVS_ShadowMap)
	{
		m_pVS_ShadowMap = pVS_ShadowMap;
	}

	auto pPS_ShadowMap = g_graphicsAPI().CreatePixelShader(L"./shaders/Gbuffer.hlsl", "shadow_map_pixel_main");
	if (pPS_ShadowMap)
	{
		m_pPS_ShadowMap = pPS_ShadowMap;
	}

	//FORWARD SHADER
	auto pVS_Forward = g_graphicsAPI().CreateVertexShader(L"./shaders/ForwardShading.hlsl", "vertex_main");
	if (pVS_Forward)
	{
		m_pVS_Forward = pVS_Forward;
	}

	auto pPS_Forward = g_graphicsAPI().CreatePixelShader(L"./shaders/ForwardShading.hlsl", "pixel_main");
	if (pPS_Forward)
	{
		m_pPS_Forward = pPS_Forward;
	}

}

void Renderer::InitInputLayout()
{

	Vector<D3D11_INPUT_ELEMENT_DESC> inputElementDesc = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,	0,  0,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR"	  , 0, DXGI_FORMAT_R32G32B32_FLOAT,	0, 12,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL"  , 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT" , 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT   ,	0, 48,	D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	m_pInputLayout = g_graphicsAPI().CreateInputLayout(inputElementDesc, m_pGBuffer_VS);
	if (!m_pInputLayout)
	{
		SHOW_ERROR(L"Couldn't create input layout for gbuffer");
		return;
	}

	m_pDefLightingInputLayout = g_graphicsAPI().CreateInputLayout(inputElementDesc, m_pVS_SSFS);
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
													//Matriz de mundo se hace identidad
	m_WVP.view = CAMERA->getViewMatrix();											//Matriz de vista se obtiene de cámara
	m_WVP.projection = CAMERA->getProjectionMatrix();								//Matriz de proyección se obtiene de cámara
	m_WVP.viewDir = CAMERA->GetViewDir();

	


	m_WVP.ligthView.LookAt(Vector3(20.0f,20.0f, 5.0f),Vector3(0, 0, 0),Vector3(0, 1, 0));
	m_WVP.lightProjection.SetOrthographic(-5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 100.0f);

	m_WVP.world.Transpose();														//Se transpone
	m_WVP.view.Transpose();															//Se transpone
	m_WVP.projection.Transpose();													//Se transpone
	m_WVP.ligthView.Transpose();
	m_WVP.lightProjection.Transpose();
	
	m_WVP.lightPosition = Vector3(3.0f, 3.0f, 3.0f);
}

void Renderer::InitConstantBuffer()
{

	Vector<char> matrix_data;
	matrix_data.resize(sizeof(MatrixCollection));
	memcpy(matrix_data.data(),&m_WVP,sizeof(MatrixCollection));

	m_pCB_WVP = g_graphicsAPI().CreateConstantBuffer(matrix_data);
	if (!m_pCB_WVP)
	{
		SHOW_ERROR(L"Couldn't constant buffer");
		return;
	}

}

void Renderer::InitRasterizerStates()
{

	ID3D11RasterizerState1* pRasterState = nullptr;

	m_RasterStates.insert({	RasterStates::DEFAULT,		pRasterState});
	m_RasterStates.insert({ RasterStates::WIREFRAME,	pRasterState });
	m_RasterStates.insert({ RasterStates::NO_CULLING,	pRasterState });

	CD3D11_RASTERIZER_DESC1 descRD(D3D11_DEFAULT);
	
	
	HRESULT hr;
	hr = g_graphicsAPI().m_pDevice->CreateRasterizerState1(&descRD,
																							 &m_RasterStates.at(RasterStates::DEFAULT));
	if (FAILED(hr))
	{
		SHOW_ERROR(L"Couldn't create Default Rasterizer State");
		return;
	}

	descRD = CD3D11_RASTERIZER_DESC1 (D3D11_DEFAULT);
	descRD.FillMode = D3D11_FILL_WIREFRAME;
	hr = g_graphicsAPI().m_pDevice->CreateRasterizerState1(&descRD,
																							 &m_RasterStates.at(RasterStates::WIREFRAME));
	if (FAILED(hr))
	{
		SHOW_ERROR(L"Couldn't create Wireframe Rasterizer State");
		return;
	}

	descRD = CD3D11_RASTERIZER_DESC1(D3D11_DEFAULT);
	descRD.CullMode = D3D11_CULL_NONE;
	hr = g_graphicsAPI().m_pDevice->CreateRasterizerState1(&descRD,
																						   &m_RasterStates.at(RasterStates::NO_CULLING));
	if (FAILED(hr))
	{
		SHOW_ERROR(L"Couldn't create CullNone Rasterizer State");
		return;
	}
}

void Renderer::InitSampleFilters()
{

	ID3D11SamplerState* pSamplerState = nullptr;

	m_SamplerStates.insert({ SamplerStates::POINT,			 pSamplerState});
	m_SamplerStates.insert({ SamplerStates::LINEAR,			 pSamplerState });
	m_SamplerStates.insert({ SamplerStates::ANISOTROPIC, pSamplerState });

	CD3D11_SAMPLER_DESC descSS(D3D11_DEFAULT);
	descSS.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	descSS.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;

	descSS.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	
	HRESULT hr;
	hr = g_graphicsAPI().m_pDevice->CreateSamplerState(&descSS,
																					 &m_SamplerStates.at(SamplerStates::POINT));
	if (FAILED(hr))
	{
		SHOW_ERROR(L"Couldn't create Point Sampler State");
		return;
	}

	descSS.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	hr = g_graphicsAPI().m_pDevice->CreateSamplerState(&descSS,
																					 &m_SamplerStates.at(SamplerStates::LINEAR));
	if (FAILED(hr))
	{
		SHOW_ERROR(L"Couldn't create Linear Sampler State");
		return;
	}

	descSS.Filter = D3D11_FILTER_ANISOTROPIC;
	descSS.MaxAnisotropy = 16;
	hr = g_graphicsAPI().m_pDevice->CreateSamplerState(&descSS,
																					 &m_SamplerStates.at(SamplerStates::ANISOTROPIC));
	if (FAILED(hr))
	{
		SHOW_ERROR(L"Couldn't create Anisotropic Sampler State");
		return;
	}
}

void Renderer::InitGBuffer(int width, int height)
{

	m_GBuffer.resize(4);

	//Position texture
	m_GBuffer[0].m_pTexture = g_graphicsAPI().CreateTexture(width,
																							  height,
																							  DXGI_FORMAT_R32G32B32A32_FLOAT,
																							  D3D11_USAGE_DEFAULT,
																							  D3D11_BIND_RENDER_TARGET |
																							  D3D11_BIND_SHADER_RESOURCE,
																							  0,
																							  1,
																							  &m_GBuffer[0].m_pSRV,
																							  &m_GBuffer[0].m_pRTV);
	//Normal texture
	m_GBuffer[1].m_pTexture = g_graphicsAPI().CreateTexture(width,
																								height,
																								DXGI_FORMAT_R8G8B8A8_UNORM,
																								D3D11_USAGE_DEFAULT,
																								D3D11_BIND_RENDER_TARGET |
																								D3D11_BIND_SHADER_RESOURCE,
																								0,
																								1,
																								&m_GBuffer[1].m_pSRV,
																								&m_GBuffer[1].m_pRTV);
	//Color texture
	m_GBuffer[2].m_pTexture = g_graphicsAPI().CreateTexture(width,
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
	m_GBuffer[3].m_pTexture = g_graphicsAPI().CreateTexture(width,
																								height,
																								DXGI_FORMAT_R16_UNORM,
																								D3D11_USAGE_DEFAULT,
																								D3D11_BIND_RENDER_TARGET |
																								D3D11_BIND_SHADER_RESOURCE,
																								0,
																								1,
																								&m_GBuffer[3].m_pSRV,
																								&m_GBuffer[3].m_pRTV);
	//ShadowMap
	m_dsShadowMap.m_pTexture = g_graphicsAPI().CreateTexture(width,
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
	//CreateDefaultSRV(0x8080FF, DefaultTextures::NORMAL, DXGI_FORMAT_R8G8B8A8_UNORM);
	CreateDefaultSRV(0xFF8080, DefaultTextures::NORMAL, DXGI_FORMAT_R8G8B8A8_UNORM);
}


void Renderer::SetPasses()
{
	//Todo lo que se podría renderizar aquí de los RTV puede ir aquí.
	
	//Primero Opacos
	//Luego objetos transparentes
	//´Primero el personaje, luego sus lentes y se ve bien perfecto

	// Primero -> !!!!!!!!!!!Opacos
	// Transparentes -> ->SEGUNDOOOOOOOOOOOOOOO -> CULLING ATRÁS Y LUEGO ADELANTE.
	// Atrás -> pa adelante.

	// Pase de transmisión tal vez.

	// Propiedad del material -> PONERSELO Y DE ACUERDO AL TIPO DE OPACO O TRANSPARENCIA.
	// Por ejemplo -> Si la textura tiene un nivel de alpha diferente de 1.
	// 
	// DIBUJALS TODOS PRIMERO Y LUEGO LO DIBUJAS DESPUÉS LOS OTROS.
	// OPACOS.
	// TRANSPARENTES.
	
	//Una manera puede ser checando la textura.

	//Opacos.
	//Transmisión. .-> qué tanto permito que pase la luz. -> similar a la refracción.
	//Transparente.

	SetShadowPass();
	
	if (m_currentShadingState == ShadingState::DEFERRED)
	{
		SetGeometryPass();
		SetSSAOPass();
		SetLightingPass();
	}

	// Definir la cámara que se va a usar.
	if (m_currentShadingState == ShadingState::FORWARD)
	{
		SetForwardPass();
	}
}

void Renderer::SetForwardPass()
{
	if (m_pWorld.expired())
	{
		SHOW_ERROR(L"Couldn't set Geometry Pass :(")
			return;
	}

	auto WORLD = m_pWorld.lock();

	FloatColor tempColor = Color{ 0, 0, 0, 0 };
	float clearColor[4] = { tempColor.r,tempColor.g , tempColor.b,tempColor.a };

	g_graphicsAPI().m_pDeviceContext->ClearRenderTargetView(g_graphicsAPI().m_pBackBufferRTV,
																													clearColor);

	g_graphicsAPI().m_pDeviceContext->ClearDepthStencilView(g_graphicsAPI().m_pBackBufferDSV,
																													D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
																													1.0f, 0);
	
	g_graphicsAPI().m_pDeviceContext->OMSetRenderTargets(1,
		&g_graphicsAPI().m_pBackBufferRTV,
		g_graphicsAPI().m_pBackBufferDSV);
	
	g_graphicsAPI().m_pDeviceContext->VSSetShader(m_pVS_Forward->m_pVertexShader,
		nullptr,
		0);
	
	g_graphicsAPI().m_pDeviceContext->PSSetShader(m_pPS_Forward->m_pPixelShader,
		nullptr,
		0);

	for (const auto& actor : WORLD->getActors())
	{
		auto character = std::dynamic_pointer_cast<Character>(actor);

		if (character)
		{
			RenderActor(character, true);
		}
	}
}

void Renderer::RenderActor(const WPtr<Character>& character, bool bDrawWithTextures)
{
	
	auto CHAR = character.lock();
	auto CAMERA = m_pCamera.lock();

	ID3D11ShaderResourceView* pNullSRV = nullptr;

	g_graphicsAPI().m_pDeviceContext->IASetInputLayout(m_pInputLayout);

	g_graphicsAPI().m_pDeviceContext->IASetPrimitiveTopology(
		static_cast<D3D11_PRIMITIVE_TOPOLOGY>(CHAR->m_model->m_meshes[0].topology));

	UINT stride = sizeof(MODEL_VERTEX);
	UINT offset = 0;

	g_graphicsAPI().m_pDeviceContext->IASetVertexBuffers(0, 1,
		&CHAR->m_model->m_pVertexBuffer->m_pBuffer,
		&stride, &offset);
	g_graphicsAPI().m_pDeviceContext->IASetIndexBuffer(CHAR->m_model->m_pIndexBuffer->m_pBuffer,
		DXGI_FORMAT_R32_UINT, 0);

	g_graphicsAPI().m_pDeviceContext->PSSetSamplers(0, 1, &m_SamplerStates.at(SamplerStates::POINT));
	g_graphicsAPI().m_pDeviceContext->PSSetSamplers(1, 1, &m_SamplerStates.at(SamplerStates::LINEAR));
	g_graphicsAPI().m_pDeviceContext->PSSetSamplers(2, 1, &m_SamplerStates.at(SamplerStates::ANISOTROPIC));

	//Se podría hacer esto dentro del renderer normal
	m_WVP.world = CHAR->getLocalTransform().getMatrix();
	m_WVP.world.Transpose();

	Vector<char> matrix_data;
	matrix_data.resize(sizeof(MatrixCollection));
	memcpy(&matrix_data[0], &m_WVP, sizeof(MatrixCollection));

	g_graphicsAPI().m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pCB_WVP->m_pBuffer);
	g_graphicsAPI().m_pDeviceContext->PSSetConstantBuffers(0, 1, &m_pCB_WVP->m_pBuffer);

	g_graphicsAPI().m_pDeviceContext->RSSetState(m_RasterStates.at(RasterStates::DEFAULT));
	g_graphicsAPI().writeToBuffer(m_pCB_WVP, matrix_data);

	for (size_t i = 0; i < CHAR->m_model->m_meshes.size(); ++i)
	{
		if (bDrawWithTextures)
		{
			//ALBEDO
			if (CHAR->m_model->m_meshes[i].material.m_filePaths.at(TextureType::ALBEDO).empty())
			{
				g_graphicsAPI().m_pDeviceContext->PSSetShaderResources(0, 1, &g_renderer().m_DefaultTextures.at(DefaultTextures::WHITE));
			}
			else
			{
				g_graphicsAPI().m_pDeviceContext->PSSetShaderResources(0, 1, &CHAR->m_model->m_meshes[i].material.getAlbedo()->m_pSRV);
			}

			//NORMAL
			if (CHAR->m_model->m_meshes[i].material.m_filePaths.at(TextureType::NORMALS).empty())
			{
				g_graphicsAPI().m_pDeviceContext->PSSetShaderResources(1, 1, &g_renderer().m_DefaultTextures.at(DefaultTextures::NORMAL));
			}
			else
			{
				g_graphicsAPI().m_pDeviceContext->PSSetShaderResources(1, 1, &CHAR->m_model->m_meshes[i].material.getNormals()->m_pSRV);
			}

			//ROUGHNESS
			if (CHAR->m_model->m_meshes[i].material.m_filePaths.at(TextureType::ROUGHNESS).empty())
			{
				g_graphicsAPI().m_pDeviceContext->PSSetShaderResources(2, 1, &g_renderer().m_DefaultTextures.at(DefaultTextures::BLACK));

			}
			else
			{
				g_graphicsAPI().m_pDeviceContext->PSSetShaderResources(2, 1, &CHAR->m_model->m_meshes[i].material.getRoughness()->m_pSRV);
			}

			//METALLIC
			if (CHAR->m_model->m_meshes[i].material.m_filePaths.at(TextureType::METALLIC).empty())
			{
				g_graphicsAPI().m_pDeviceContext->PSSetShaderResources(3, 1, &g_renderer().m_DefaultTextures.at(DefaultTextures::WHITE));
			}
			else
			{
				g_graphicsAPI().m_pDeviceContext->PSSetShaderResources(3, 1, &CHAR->m_model->m_meshes[i].material.getMetallic()->m_pSRV);
			}
		}

		g_graphicsAPI().m_pDeviceContext->DrawIndexed(CHAR->m_model->m_meshes[i].numIndices,
			CHAR->m_model->m_meshes[i].baseIndex,
			CHAR->m_model->m_meshes[i].baseVertex);
	}
	
}

void Renderer::RenderShadows(const WPtr<Character>& character)
{
	if (character.expired() || m_pCamera.expired())
	{
		SHOW_ERROR(L"Couldn't render Actor :(")
			return;
	}
	auto CHAR = character.lock();

	auto CAMERA = m_pCamera.lock();

	g_graphicsAPI().m_pDeviceContext->IASetInputLayout(m_pInputLayout);

	g_graphicsAPI().m_pDeviceContext->IASetPrimitiveTopology(
		static_cast<D3D11_PRIMITIVE_TOPOLOGY>(CHAR->m_model->m_meshes[0].topology));

	UINT stride = sizeof(MODEL_VERTEX);
	UINT offset = 0;

	g_graphicsAPI().m_pDeviceContext->IASetVertexBuffers(0, 1,
																						 &CHAR->m_model->m_pVertexBuffer->m_pBuffer,
																						 &stride, &offset);
	g_graphicsAPI().m_pDeviceContext->IASetIndexBuffer(CHAR->m_model->m_pIndexBuffer->m_pBuffer,
																					 DXGI_FORMAT_R32_UINT, 0);

	m_WVP.world = CHAR->getLocalTransform().getMatrix();
	m_WVP.world.Transpose();

	Vector<char> matrix_data;
	matrix_data.resize(sizeof(MatrixCollection));
	memcpy(matrix_data.data(), &m_WVP, sizeof(MatrixCollection));

	g_graphicsAPI().m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pCB_WVP->m_pBuffer);
	g_graphicsAPI().m_pDeviceContext->PSSetConstantBuffers(0, 1, &m_pCB_WVP->m_pBuffer);

	g_graphicsAPI().m_pDeviceContext->RSSetState(m_RasterStates.at(RasterStates::DEFAULT));
	g_graphicsAPI().writeToBuffer(m_pCB_WVP, matrix_data);

	for (size_t i = 0; i < CHAR->m_model->m_meshes.size(); ++i)
	{
		g_graphicsAPI().m_pDeviceContext->DrawIndexed(CHAR->m_model->m_meshes[i].numIndices,
			CHAR->m_model->m_meshes[i].baseIndex,
			CHAR->m_model->m_meshes[i].baseVertex);
	}
}

//Se podría pasarse como
void Renderer::CreateDefaultSRV(UINT value, DefaultTextures::E defaultTextureType, DXGI_FORMAT format)
{

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

	hr = g_graphicsAPI().m_pDevice->CreateTexture2D(&desc, &initData, &tex);

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

	hr = g_graphicsAPI().m_pDevice->CreateShaderResourceView(tex, &srvDesc, &pSRV);
	if (FAILED(hr))
	{
		tex->Release();
		SHOW_ERROR(L"srv for texture couldn't be created.");
		return;
	}

	m_DefaultTextures.insert({ defaultTextureType, pSRV });
}

// 
// Indice asociado a cual hueso está ligado
// Y un peso flotatnte que indica el porcentaje de cambio que se aplica el movimiento de hueso al movimiento del vértices.
// Cinemática directa e inversa.
// Cinemática directa es el brazo de robot.
// 
// Cinemática Inversa
// La resolución de los huesos se hace del final hacia atrás.
// 
// Constraints
// Ejemplo-> se le ponen limitantes.
// 
// Esqueleto: Tendrá constraints para tener la solución correcta al final del IK. 
//						Se ve como si fuera 2D.
//						Máximo de 4 huesos.
//						Skinning, Pesos, Joints. 
//						En un modelo, limitamos el número de huesos que pueden afectar a unvértice.
//						Tradicionalmente 4 huesos por vértices.
//						Cada vértices puede ser por 4 huesos.
// 
// La base de cómo construir la estructura del esqueleto. Cómo estar ligado.
// 
// La animación -> Es como tener un timeline, es como un pentagrama. De música. Por cada hueso en el modelos
//								 Se pondrá una línea en este timeline. Es un poligrama. Las notas son los keyframes. 
//								 Este hueso tien esta posición y giro.
//								 La curva es como una interpolación.
//								 A + (B - A) * T
//								 Interpolación:
//								 Interpolación esférica -> son las curvas
//								 Interpolación lineal para posición.
//								 Quaterniones para interpolaciónes esféricas.
// 
// Crystal Dynamics -> Tomb Raider el I -> Legacy of Kane Soul River
// 
// Así es una mezcla de animación con collider.
// 
// Esto viene de la robótica.
// 
// Sistemas de parkour son IKs y ligados a mecanismos de animación.
// 
// Assimp tiene para la animación


void Renderer::SetShadowPass()
{
	if (m_pWorld.expired())
	{
		SHOW_ERROR(L"Couldn't set Shadow Pass :(")
			return;
	}
	auto WORLD = m_pWorld.lock();

	g_graphicsAPI().m_pDeviceContext->ClearDepthStencilView(m_dsShadowMap.m_pDSV,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f, 0);

	const Vector<ID3D11RenderTargetView*> renderTargets =
	{
		nullptr,
		nullptr,
		nullptr
	};

	g_graphicsAPI().m_pDeviceContext->VSSetShader(m_pVS_ShadowMap->m_pVertexShader,
		nullptr,
		0);
	g_graphicsAPI().m_pDeviceContext->PSSetShader(m_pPS_ShadowMap->m_pPixelShader,
		nullptr,
		0);
	g_graphicsAPI().m_pDeviceContext->OMSetRenderTargets(renderTargets.size(),
																						 renderTargets.data(),
																						 m_dsShadowMap.m_pDSV);

	auto CAMERA = m_pCamera.lock();

	m_WVP.view = CAMERA->getViewMatrix();
	m_WVP.projection = CAMERA->getProjectionMatrix();
	m_WVP.viewDir = CAMERA->GetViewDir();

	//GPASS
	m_WVP.projection.Transpose();
	m_WVP.view.Transpose();

	for (auto& actor : WORLD->getActors())
	{
		auto character = std::dynamic_pointer_cast<Character>(actor);

		if (character)
		{
			RenderActor(character,false);
		}
	}
	
}

void Renderer::SetGeometryPass()
{
	if (m_pWorld.expired())
	{
		SHOW_ERROR(L"Couldn't set Geometry Pass :(")
			return;
	}

	auto WORLD = m_pWorld.lock();
	
	FloatColor tempColor = Color{ 0, 0, 0, 0 };
	float clearColor2[4] = { tempColor.r,tempColor.g , tempColor.b,tempColor.a };
	
	g_graphicsAPI().m_pDeviceContext->ClearRenderTargetView(g_graphicsAPI().m_pBackBufferRTV,
																								clearColor2);
	g_graphicsAPI().m_pDeviceContext->ClearRenderTargetView(m_GBuffer[0].m_pRTV,
																								clearColor2);
	g_graphicsAPI().m_pDeviceContext->ClearRenderTargetView(m_GBuffer[1].m_pRTV,
																								clearColor2);
	g_graphicsAPI().m_pDeviceContext->ClearRenderTargetView(m_GBuffer[2].m_pRTV,
																								clearColor2);
	g_graphicsAPI().m_pDeviceContext->ClearRenderTargetView(m_GBuffer[3].m_pRTV,
																								clearColor2);

	g_graphicsAPI().m_pDeviceContext->ClearDepthStencilView(g_graphicsAPI().m_pBackBufferDSV,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f, 0);

	const Vector<ID3D11RenderTargetView*> renderTargets =
	{
		m_GBuffer[0].m_pRTV,
		m_GBuffer[1].m_pRTV,
		m_GBuffer[2].m_pRTV
	};

	for (const auto& actor : WORLD->getActors())
	{
		auto character = std::dynamic_pointer_cast<Character>(actor);

		if (character)
		{
			g_graphicsAPI().m_pDeviceContext->OMSetRenderTargets(renderTargets.size(),
				renderTargets.data(),
				g_graphicsAPI().m_pBackBufferDSV);
			g_graphicsAPI().m_pDeviceContext->VSSetShader(m_pGBuffer_VS->m_pVertexShader,
				nullptr,
				0);
			g_graphicsAPI().m_pDeviceContext->PSSetShader(m_pGBuffer_PS->m_pPixelShader,
				nullptr,
				0);
			RenderActor(character, true);
		}
	}
}

void Renderer::SetSSAOPass()
{

	ID3D11ShaderResourceView* pNullSRV = nullptr;

	g_graphicsAPI().m_pDeviceContext->VSSetShader(m_pVS_SSFS->m_pVertexShader,
																			nullptr,
																			0);
	g_graphicsAPI().m_pDeviceContext->PSSetShader(m_pPS_AO->m_pPixelShader,
																			nullptr,
																			0);

	const Vector<ID3D11RenderTargetView*> renderTargets =
	{
		m_GBuffer[3].m_pRTV,
		nullptr,
		nullptr
	};
	
	g_graphicsAPI().m_pDeviceContext->OMSetRenderTargets(renderTargets.size(),
																						 renderTargets.data(),
																						 nullptr);
	
	g_graphicsAPI().m_pDeviceContext->RSSetState(m_RasterStates.at(RasterStates::NO_CULLING));

	g_graphicsAPI().m_pDeviceContext->PSSetShaderResources(0, 1, &m_GBuffer[0].m_pSRV);
	g_graphicsAPI().m_pDeviceContext->PSSetShaderResources(1, 1, &m_GBuffer[1].m_pSRV);
	g_graphicsAPI().m_pDeviceContext->PSSetShaderResources(2, 1, &m_GBuffer[2].m_pSRV);
	g_graphicsAPI().m_pDeviceContext->PSSetShaderResources(3, 1, &pNullSRV);

	g_graphicsAPI().m_pDeviceContext->IASetInputLayout(m_pDefLightingInputLayout);
	
	g_graphicsAPI().m_pDeviceContext->Draw(3, 0);

	g_graphicsAPI().m_pDeviceContext->PSSetShaderResources(0, 1, &pNullSRV);
	g_graphicsAPI().m_pDeviceContext->PSSetShaderResources(1, 1, &pNullSRV);
	g_graphicsAPI().m_pDeviceContext->PSSetShaderResources(2, 1, &pNullSRV);
	g_graphicsAPI().m_pDeviceContext->PSSetShaderResources(3, 1, &pNullSRV);

}

void Renderer::SetLightingPass()
{

	ID3D11ShaderResourceView* pNullSRV = nullptr;

	g_graphicsAPI().m_pDeviceContext->VSSetShader(m_pVS_SSFS->m_pVertexShader,
		nullptr,
		0);
	g_graphicsAPI().m_pDeviceContext->PSSetShader(m_pPS_DefLighting->m_pPixelShader,
		nullptr,
		0);

	const Vector<ID3D11RenderTargetView*> renderTargets =
	{
		g_graphicsAPI().m_pBackBufferRTV,
		nullptr,
		nullptr
	};

	g_graphicsAPI().m_pDeviceContext->OMSetRenderTargets(renderTargets.size(),
																							renderTargets.data(),
																							nullptr);

	g_graphicsAPI().m_pDeviceContext->RSSetState(m_RasterStates.at(RasterStates::NO_CULLING));

	g_graphicsAPI().m_pDeviceContext->PSSetShaderResources(0, 1, &m_GBuffer[0].m_pSRV);
	g_graphicsAPI().m_pDeviceContext->PSSetShaderResources(1, 1, &m_GBuffer[1].m_pSRV);
	g_graphicsAPI().m_pDeviceContext->PSSetShaderResources(2, 1, &m_GBuffer[2].m_pSRV);
	g_graphicsAPI().m_pDeviceContext->PSSetShaderResources(3, 1, &m_GBuffer[3].m_pSRV);
	g_graphicsAPI().m_pDeviceContext->PSSetShaderResources(4, 1, &m_dsShadowMap.m_pSRV);

	g_graphicsAPI().m_pDeviceContext->IASetInputLayout(m_pDefLightingInputLayout);
	g_graphicsAPI().m_pDeviceContext->Draw(3, 0);//DRAWWW

	g_graphicsAPI().m_pDeviceContext->PSSetShaderResources(0, 1, &pNullSRV);
	g_graphicsAPI().m_pDeviceContext->PSSetShaderResources(1, 1, &pNullSRV);
	g_graphicsAPI().m_pDeviceContext->PSSetShaderResources(2, 1, &pNullSRV);
	g_graphicsAPI().m_pDeviceContext->PSSetShaderResources(3, 1, &pNullSRV);
	g_graphicsAPI().m_pDeviceContext->PSSetShaderResources(4, 1, &pNullSRV);

}

Renderer& g_renderer()
{
	return Renderer::GetInstance();
}
