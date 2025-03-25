#define SDL_MAIN_USE_CALLBACKS 1
//#define WIDTH 1920
#define WIDTH 1280
//#define HEIGHT 1080
#define HEIGHT 720

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "Model.h"
#include <iostream>
#include "MathObjects.h"
#include "Texture.h"
#include "GraphicsAPI.h"

struct MatrixCollection
{
	Matrix4 world;
	Matrix4 view;
	Matrix4 projection;
};

SDL_Window* g_pWindow = nullptr;

UPtr<GraphicsAPI> g_pGraphicsAPI;
UPtr<VertexShader> g_pVertexShader;
UPtr<PixelShader> g_pPixelShader;

Model g_myModel;
Model g_FloorModel;
Model g_HumanModel;
Model g_AmbulanceModel;

Texture g_myTexture;
Texture g_FloorTexture;
Texture g_HumanTexture;
Texture g_AmbulanceTexture;

Texture g_dsReflection;
Texture g_rtReflection;

ID3D11InputLayout* g_pInputLayout = nullptr;

ID3D11RasterizerState1* g_pRS_Default = nullptr;
ID3D11RasterizerState1* g_pRS_Wireframe = nullptr;
ID3D11RasterizerState1* g_pRS_No_Cull = nullptr;

ID3D11SamplerState* g_pSS_Point = nullptr;
ID3D11SamplerState* g_pSS_Linear = nullptr;
ID3D11SamplerState* g_pSS_Anisotropic = nullptr;

UPtr<GraphicsBuffer> g_pCB_WVP;
MatrixCollection g_WVP;

Camera g_Camera;

static float cameraMovSpeed = 20.0f;
static float cameraRotSpeed = 0.4f;

Uint64 g_lastTick = 0;
Uint64 g_currentTick = 0;
Uint64 g_elapsedTick = 0;
float g_deltaTime = 0.0f;

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
	

#pragma region SDL3_SETUP
	//---------------SDL SET UP!!-----------------
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}

	g_pWindow = SDL_CreateWindow("Soft Rasterizer",
								WIDTH, 
								HEIGHT,
								SDL_WINDOW_RESIZABLE);
	
	auto pWndHandle = SDL_GetPointerProperty(	SDL_GetWindowProperties(g_pWindow),
											SDL_PROP_WINDOW_WIN32_HWND_POINTER, 
											nullptr);
	if (pWndHandle)
	{
		g_pGraphicsAPI = make_unique<GraphicsAPI>(pWndHandle);
		if(!g_pGraphicsAPI)
		{
			return SDL_APP_FAILURE;
		}
	}
#pragma endregion SDL3_SETUP

#pragma region VERTEX_SHADER_CREATION
	//---------------VERTEX SHADER!---------------
	g_pVertexShader = g_pGraphicsAPI->CreateVertexShader(L"BasicShader.hlsl", "vertex_main");
	if (!g_pVertexShader)
	{
		return SDL_APP_FAILURE;
	}
#pragma endregion VERTEX_SHADER_CREATION

#pragma region PIXEL_SHADER_CREATION
	//---------------PIXEL SHADER!---------------
	g_pPixelShader = g_pGraphicsAPI->CreatePixelShader(L"BasicShader.hlsl", "pixel_main");
	if(!g_pPixelShader)
	{
		return SDL_APP_FAILURE;
	}
#pragma endregion PIXEL_SHADER_CREATION

	//---------------INPUT VERTEX BUFFER TO THE SHADER---------------
	Vector<D3D11_INPUT_ELEMENT_DESC> inputElementDesc = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,	0,  0,	D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "COLOR"	, 0, DXGI_FORMAT_R32G32B32_FLOAT,	0, 12,	D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,	    0, 24,	D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	//---------------INPUT LAYOUT!---------------
	g_pInputLayout = g_pGraphicsAPI->CreateInputLayout(inputElementDesc, g_pVertexShader);
	if (!g_pInputLayout)
	{
		return SDL_APP_FAILURE;
	}

	//---------------CAMERA SET UP---------------
	g_Camera.SetLookAt(Vector3(0, 0, -6.0f), Vector3(0, 0, 0), Vector3(0, 1, 0));		//Setea la matriz de la cámara
	g_Camera.SetPerspective(3.141592653f/4.0f,WIDTH,HEIGHT,0.1f,100.0f);			//Sete la matriz de la perspectiva

	//---------------WORLD SETUP---------------
	g_WVP.world.Identity();															//Matriz de mundo se hace identidad
	g_WVP.view = g_Camera.getViewMatrix();											//Matriz de vista se obtiene de cámara
	g_WVP.projection = g_Camera.getProjectionMatrix();								//Matriz de proyección se obtiene de cámara
	
	g_WVP.world.Transpose();														//Se transpone
	g_WVP.view.Transpose();															//Se transpone
	g_WVP.projection.Transpose();													//Se transpone

	//---------------CONSTANT BUFFER FOR---------------
	Vector<char> matrix_data;														//Se pasa al constant buffer para el shader.
	matrix_data.resize(sizeof(g_WVP));
	memcpy(matrix_data.data(), &g_WVP, sizeof(g_WVP));
	g_pCB_WVP = g_pGraphicsAPI->CreateConstantBuffer(matrix_data);

#pragma region RASTERIZER_STATES
	//---------------RASTERIZER STATES SET UP---------------
	CD3D11_RASTERIZER_DESC1 descRD(D3D11_DEFAULT);
	g_pGraphicsAPI->m_pDevice->CreateRasterizerState1(&descRD, &g_pRS_Default);

	descRD.FillMode = D3D11_FILL_WIREFRAME;
	g_pGraphicsAPI->m_pDevice->CreateRasterizerState1(&descRD, &g_pRS_Wireframe);

	descRD.CullMode = D3D11_CULL_NONE;
	g_pGraphicsAPI->m_pDevice->CreateRasterizerState1(&descRD, &g_pRS_No_Cull);

#pragma endregion RASTERIZER_STATES

#pragma region SAMPLER_FILTERS
	//---------------SAMPLER FILTERS SET UP---------------
	CD3D11_SAMPLER_DESC descSS(D3D11_DEFAULT);
	descSS.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	descSS.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;

	descSS.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	g_pGraphicsAPI->m_pDevice->CreateSamplerState(&descSS, &g_pSS_Point);

	descSS.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	g_pGraphicsAPI->m_pDevice->CreateSamplerState(&descSS, &g_pSS_Linear);
	
	descSS.Filter = D3D11_FILTER_ANISOTROPIC;
	descSS.MaxAnisotropy = 16;
	g_pGraphicsAPI->m_pDevice->CreateSamplerState(&descSS, &g_pSS_Anisotropic);
#pragma endregion SAMPLER_FILTERS

#pragma region LOAD_MODELS
	//---------------LOAD DINOSAUR MODEL---------------
	g_myModel.LoadFromFile("rex.obj", g_pGraphicsAPI);
	Image srcImage;
	srcImage.decode("Rex_C.bmp");
	g_myTexture.createImage(srcImage, g_pGraphicsAPI);

	g_rtReflection.m_pTexture = g_pGraphicsAPI->CreateTexture(WIDTH, HEIGHT, DXGI_FORMAT_B8G8R8A8_UNORM,
		D3D11_USAGE_DEFAULT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
		0, 1, &g_rtReflection.m_pSRV, &g_rtReflection.m_pRTV);

	//---------------LOAD FLOOR MODEL---------------
	g_FloorModel.LoadFromFile("disc.obj", g_pGraphicsAPI);
	Image srcImage2;
	srcImage2.decode("Terrain1.bmp");
	g_FloorTexture.createImage(srcImage2, g_pGraphicsAPI);

	g_dsReflection.m_pTexture = g_pGraphicsAPI->CreateTexture(WIDTH, HEIGHT, DXGI_FORMAT_D24_UNORM_S8_UINT,
		D3D11_USAGE_DEFAULT, D3D11_BIND_DEPTH_STENCIL,
		0, 1, nullptr, nullptr, &g_dsReflection.m_pDSV);

	//---------------LOAD HUMAN--------------------
	g_HumanModel.LoadFromFile("ManModel.obj",g_pGraphicsAPI);
	Image humanModelImage;
	humanModelImage.decode("manText.bmp");
	g_HumanTexture.createImage(humanModelImage, g_pGraphicsAPI);

	//---------------LOAD AMBULANCE--------------------
	g_AmbulanceModel.LoadFromFile("ambulance2.obj",g_pGraphicsAPI);
	Image ambulanceModelImage;
	ambulanceModelImage.decode("colormap2.bmp");
	g_AmbulanceTexture.createImage(ambulanceModelImage, g_pGraphicsAPI);

	
	

#pragma endregion LOAD_MODELS

	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event * event)
{
	if (event->type == SDL_EVENT_QUIT) {
		return SDL_APP_SUCCESS;
	}
#pragma region CAMERA_MOVEMENT
	if (event->key.scancode == SDL_SCANCODE_W)
	{
		//g_Camera.Move(g_Camera.GetForward() * movSpeed);
		g_Camera.Move(Vector3{ 0.0,0.0f,-1.0f });

		g_WVP.world.Identity();
		g_WVP.view = g_Camera.getViewMatrix();

		g_WVP.view.Transpose();
		g_WVP.world.Transpose();
	}
	if (event->key.scancode == SDL_SCANCODE_S)
	{	
		g_Camera.Move(Vector3{ 0.0,0.0f,1.0f });

		g_WVP.world.Identity();
		g_WVP.view = g_Camera.getViewMatrix();

		g_WVP.view.Transpose();
		g_WVP.world.Transpose();
	}
	if (event->key.scancode == SDL_SCANCODE_A)
	{
		g_Camera.Move(Vector3{ 1.0,0.0f,0.0f });

		g_WVP.world.Identity();
		g_WVP.view = g_Camera.getViewMatrix();

		g_WVP.view.Transpose();
		g_WVP.world.Transpose();
	}
	if (event->key.scancode == SDL_SCANCODE_D)
	{
		g_Camera.Move(Vector3{ -1.0,0.0f,0.0f });

		g_WVP.world.Identity();
		g_WVP.view = g_Camera.getViewMatrix();

		g_WVP.view.Transpose();
		g_WVP.world.Transpose();
	}
	if (event->key.scancode == SDL_SCANCODE_Q)
	{
		g_Camera.Move(Vector3{ 0.0,1.0f,0.0f });

		g_WVP.world.Identity();
		g_WVP.view = g_Camera.getViewMatrix();

		g_WVP.view.Transpose();
		g_WVP.world.Transpose();
	}
	if (event->key.scancode == SDL_SCANCODE_E)
	{
		g_Camera.Move(Vector3{ 0.0,-1.0f,0.0f });

		g_WVP.view = g_Camera.getViewMatrix();
		
		g_WVP.view.Transpose();
		g_WVP.world.Transpose();
	}

#pragma endregion CAMERA_MOVEMENT

#pragma region CAMER_ROTATION
	if (event->key.scancode == SDL_SCANCODE_UP)
	{

		g_Camera.RotateX(cameraRotSpeed);
		g_WVP.view = g_Camera.getViewMatrix();

		g_WVP.view.Transpose();
		g_WVP.world.Transpose();
	}
	if (event->key.scancode == SDL_SCANCODE_DOWN)
	{

		g_Camera.RotateX(-cameraRotSpeed);
		g_WVP.view = g_Camera.getViewMatrix();

		g_WVP.view.Transpose();
		g_WVP.world.Transpose();
	}
	if (event->key.scancode == SDL_SCANCODE_LEFT)
	{

		g_Camera.RotateY(cameraRotSpeed);
		g_WVP.view = g_Camera.getViewMatrix();

		g_WVP.view.Transpose();
		g_WVP.world.Transpose();
	}
	if (event->key.scancode == SDL_SCANCODE_RIGHT)
	{

		g_Camera.RotateY(-cameraRotSpeed);
		g_WVP.view = g_Camera.getViewMatrix();

		g_WVP.view.Transpose();
		g_WVP.world.Transpose();
	}

#pragma endregion CAMER_ROTATION

	if (event->type == SDL_EVENT_MOUSE_MOTION)
	{
		float x, y;
		SDL_GetMouseState(&x,&y);

		printf("Mouse x: %f", x);
		printf("	Mouse y: %f\n", y);

	}
	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate)
{
	g_lastTick = g_currentTick;
	g_currentTick = SDL_GetTicks();
	
	g_deltaTime = (g_currentTick - g_lastTick);

	g_lastTick += g_deltaTime;

	g_elapsedTick += g_deltaTime;

	

	printf("Delta Time: %f\n",g_deltaTime);

#pragma region VIEWPORT
	D3D11_VIEWPORT vp;
	vp.Width = WIDTH;
	vp.Height = HEIGHT;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;

	g_pGraphicsAPI->m_pDeviceContext->RSSetViewports(1, &vp);
#pragma endregion VIEWPORT

#pragma region OUTPUT_MERGE_BACKBUFFER
	
	//Se setea lo que se mostrará en el Output Merge State
	g_pGraphicsAPI->m_pDeviceContext->OMSetRenderTargets(1, &g_pGraphicsAPI->m_pBackBufferRTV, g_pGraphicsAPI->m_pBackBufferDSV);

#pragma endregion OUTPUT_MERGE_BACKBUFFER

#pragma region CLEAN_RENDER_TARGET_DEPTH_STENCIL

	FloatColor tempColor = Color{ 135, 206, 235, 255 };
	float clearColor2[4] = { tempColor.r,tempColor.g , tempColor.b,tempColor.a };
	
	g_pGraphicsAPI->m_pDeviceContext->ClearRenderTargetView(g_pGraphicsAPI->m_pBackBufferRTV, clearColor2);
	
	//Limpiamos el depth stencil del backbuffer
	g_pGraphicsAPI->m_pDeviceContext->ClearDepthStencilView(g_pGraphicsAPI->m_pBackBufferDSV,
															D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 
															1.0f, 
															0);
#pragma endregion CLEAN_RENDER_TARGET_DEPTH_STENCIL	

#pragma region SET_VERTEX_SHADER_N_PIXEL_SHADER
	
	//Seteamos el vertex shader y el pixel shader
	g_pGraphicsAPI->m_pDeviceContext->VSSetShader(g_pVertexShader->m_pVertexShader, nullptr, 0);
	g_pGraphicsAPI->m_pDeviceContext->PSSetShader(g_pPixelShader->m_pPixelShader, nullptr, 0);

#pragma endregion SET_VERTEX_SHADER_N_PIXEL_SHADER

#pragma region INPUT_ASSEMBLER_LAYOUT

	//Seteamos lo que necesitamos para el input assembler
	//Seteamos el arreglo del input assembler y los valores que mandaremos al GPU
	g_pGraphicsAPI->m_pDeviceContext->IASetInputLayout(g_pInputLayout);

#pragma endregion INPUT_ASSEMBLER_LAYOUT

#pragma region SET_SAMPLERS
	g_pGraphicsAPI->m_pDeviceContext->PSSetSamplers(0, 1, &g_pSS_Point);
	g_pGraphicsAPI->m_pDeviceContext->PSSetSamplers(1, 1, &g_pSS_Linear);
	g_pGraphicsAPI->m_pDeviceContext->PSSetSamplers(2, 1, &g_pSS_Anisotropic);

#pragma endregion SET_SAMPLERS


	//------------FIRST MODEL--------------
	g_pGraphicsAPI->m_pDeviceContext->IASetPrimitiveTopology(static_cast<D3D11_PRIMITIVE_TOPOLOGY>(g_myModel.m_meshes[0].topology));
	
	

	UINT stride = sizeof(MODEL_VERTEX);
	UINT offset = 0;
	g_pGraphicsAPI->m_pDeviceContext->IASetVertexBuffers(0, 1, &g_myModel.m_pVertexBuffer->m_pBuffer, &stride, &offset);
	g_pGraphicsAPI->m_pDeviceContext->IASetIndexBuffer( g_myModel.m_pIndexBuffer->m_pBuffer, DXGI_FORMAT_R16_UINT, 0);
	g_pGraphicsAPI->m_pDeviceContext->PSSetShaderResources(0, 1, &g_myTexture.m_pSRV);

	Matrix4 translation2;
	translation2.Identity();
	translation2.Translate(Vector3(0.0, 0.0f, 0.0f));

	g_WVP.world = translation2;
	g_WVP.projection = g_Camera.getProjectionMatrix();
	g_WVP.world.Transpose();
	g_WVP.projection.Transpose();

	Vector<char> matrix_data;
	matrix_data.resize(sizeof(g_WVP));
	memcpy(matrix_data.data(), &g_WVP, sizeof(g_WVP));

	g_pGraphicsAPI->m_pDeviceContext->VSSetConstantBuffers(0, 1, &g_pCB_WVP->m_pBuffer);

	g_pGraphicsAPI->writeToBuffer(g_pCB_WVP, matrix_data);

	g_pGraphicsAPI->m_pDeviceContext->DrawIndexed(	g_myModel.m_meshes[0].numIndices, 
													g_myModel.m_meshes[0].baseIndex, 
													g_myModel.m_meshes[0].baseVertex);  //Los dibujamos de manera indexada

	//------------SECOND MODEL--------------
	UINT stride2 = sizeof(MODEL_VERTEX);
	UINT offset2 = 0;
	g_pGraphicsAPI->m_pDeviceContext->IASetPrimitiveTopology(static_cast<D3D11_PRIMITIVE_TOPOLOGY>(g_HumanModel.m_meshes[0].topology));
	g_pGraphicsAPI->m_pDeviceContext->IASetVertexBuffers(0, 1, &g_HumanModel.m_pVertexBuffer->m_pBuffer,&stride2, &offset2);
	g_pGraphicsAPI->m_pDeviceContext->IASetIndexBuffer(g_HumanModel.m_pIndexBuffer->m_pBuffer, DXGI_FORMAT_R16_UINT, 0);
	g_pGraphicsAPI->m_pDeviceContext->PSSetShaderResources(0, 1, &g_HumanTexture.m_pSRV);
	//g_pGraphicsAPI->m_pDeviceContext->RSSetState(g_pRS_Wireframe);

	Matrix4 translation1;
	translation1.Identity();
	translation1.Translate(Vector3(3.0f, 0.0f, 0.0f));

	g_WVP.world = translation1;
	g_WVP.projection = g_Camera.getProjectionMatrix();
	g_WVP.world.Transpose();
	g_WVP.projection.Transpose();

	memcpy(matrix_data.data(), &g_WVP, sizeof(g_WVP));

	g_pGraphicsAPI->m_pDeviceContext->VSSetConstantBuffers(0, 1, &g_pCB_WVP->m_pBuffer);

	g_pGraphicsAPI->writeToBuffer(g_pCB_WVP, matrix_data);

	g_pGraphicsAPI->m_pDeviceContext->DrawIndexed(g_HumanModel.m_meshes[0].numIndices, g_HumanModel.m_meshes[0].baseIndex, g_HumanModel.m_meshes[0].baseVertex);



	
	/*Matrix4 translation;
	translation.Identity();
	translation.Translate(Vector3(0.0f, 0.0f, 0.0f));

	Matrix4 translation2;
	translation2.Identity();
	translation2.Translate(Vector3(1.5f, 0.0f, -2.5f));

	Matrix4 translation3;
	translation3.Identity();
	translation3.Translate(Vector3(3.0f, 0.0f, 0.0f));

	Matrix4 rotation;
	rotation.RotateY(rotationAngle);
	
	g_WVP.world = rotation * translation;
	
	g_Camera.SetPerspective(3.141592653f / 4.0f, WIDTH, HEIGHT, 0.1f, 100.0f);

	g_WVP.projection = g_Camera.getProjectionMatrix();
	g_WVP.world.Transpose();
	g_WVP.projection.Transpose();

	Vector<char> matrix_data;
	matrix_data.resize(sizeof(g_WVP));
	memcpy(matrix_data.data(), &g_WVP, sizeof(g_WVP));
	
	g_pGraphicsAPI->m_pDeviceContext->VSSetConstantBuffers(0, 1, &g_pCB_WVP->m_pBuffer);

	g_pGraphicsAPI->m_pDeviceContext->RSSetState(g_pRS_Default);
	g_pGraphicsAPI->writeToBuffer(g_pCB_WVP,matrix_data);
	g_pGraphicsAPI->m_pDeviceContext->DrawIndexed(	g_myModel.m_meshes[0].numIndices, 
		g_myModel.m_meshes[0].baseIndex, g_myModel.m_meshes[0].baseVertex);*/ //Los dibujamos de manera indexada
	
	/*Matrix4 refScale;
	refScale.RotateY(-rotationAngle);
	g_WVP.world = refScale * rotation * translation3;
	g_WVP.world.Transpose();
	memcpy(matrix_data.data(), &g_WVP, sizeof(g_WVP));

	g_pGraphicsAPI->writeToBuffer(g_pCB_WVP, matrix_data);

	g_pGraphicsAPI->m_pDeviceContext->RSSetState(g_pRS_Default);
	g_pGraphicsAPI->m_pDeviceContext->OMSetRenderTargets(1, 
														&g_rtReflection.m_pRTV, 
														g_dsReflection.m_pDSV);

	g_pGraphicsAPI->m_pDeviceContext->DrawIndexed(	g_myModel.m_meshes[0].numIndices,
													g_myModel.m_meshes[0].baseIndex, 
													g_myModel.m_meshes[0].baseVertex);
	*/

	//------------THIRD MODEL--------------
	g_pGraphicsAPI->m_pDeviceContext->IASetPrimitiveTopology(static_cast<D3D11_PRIMITIVE_TOPOLOGY>(g_FloorModel.m_meshes[0].topology));
	g_pGraphicsAPI->m_pDeviceContext->IASetVertexBuffers(0,
		1,
		&g_FloorModel.m_pVertexBuffer->m_pBuffer,
		&stride,
		&offset);
	g_pGraphicsAPI->m_pDeviceContext->IASetIndexBuffer(
		g_FloorModel.m_pIndexBuffer->m_pBuffer,
		DXGI_FORMAT_R16_UINT,
		0);

	Matrix4 floorScale;
	floorScale.Scale(1.0f);

	g_WVP.world = floorScale * translation2;
	g_WVP.world.Transpose();
	memcpy(matrix_data.data(), &g_WVP, sizeof(g_WVP));

	g_pGraphicsAPI->m_pDeviceContext->RSSetState(g_pRS_Default);
	g_pGraphicsAPI->writeToBuffer(g_pCB_WVP, matrix_data);


	g_pGraphicsAPI->m_pDeviceContext->OMSetRenderTargets(1, &g_pGraphicsAPI->m_pBackBufferRTV, g_pGraphicsAPI->m_pBackBufferDSV);
	g_pGraphicsAPI->m_pDeviceContext->PSSetShaderResources(0, 1, &g_FloorTexture.m_pSRV);
	g_pGraphicsAPI->m_pDeviceContext->DrawIndexed(	g_FloorModel.m_meshes[0].numIndices,
													g_FloorModel.m_meshes[0].baseIndex, 
													g_FloorModel.m_meshes[0].baseVertex);

	//------------FOURTH MODEL--------------
	UINT stride3 = sizeof(MODEL_VERTEX);
	UINT offset3 = 0;
	g_pGraphicsAPI->m_pDeviceContext->IASetPrimitiveTopology(static_cast<D3D11_PRIMITIVE_TOPOLOGY>(g_AmbulanceModel.m_meshes[0].topology));
	g_pGraphicsAPI->m_pDeviceContext->IASetVertexBuffers(0, 1, &g_AmbulanceModel.m_pVertexBuffer->m_pBuffer, &stride3, &offset3);
	g_pGraphicsAPI->m_pDeviceContext->IASetIndexBuffer(g_AmbulanceModel.m_pIndexBuffer->m_pBuffer, DXGI_FORMAT_R16_UINT, 0);
	g_pGraphicsAPI->m_pDeviceContext->PSSetShaderResources(0, 1, &g_AmbulanceTexture.m_pSRV);
	//g_pGraphicsAPI->m_pDeviceContext->RSSetState(g_pRS_Wireframe);

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

	g_pGraphicsAPI->m_pSwapChain->Present(0,0);

	return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void* appstate, SDL_AppResult result)
{

	if (g_pWindow)
	{
		SDL_DestroyWindow(g_pWindow);
		g_pWindow = nullptr;
	}
	SDL_Quit();
}

