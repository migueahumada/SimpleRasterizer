#define SDL_MAIN_USE_CALLBACKS 1
#define WIDTH 1280
#define HEIGHT 720

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "Model.h"
#include <iostream>
#include "MathObjects.h"
#include "Texture.h"
#include "Camera.h"
#include "GraphicsAPI.h"
#include "AudioAPI.h"
#include "Submix.h"
#include "Audio.h"
#include <SDL3/SDL_keyboard.h>
#include "Character.h"
#include "World.h"
#include "Actor.h"
#include "VoiceCallback.h"

using std::function;

SDL_Window* g_pWindow = nullptr;
SDL_Cursor* g_pCursor = nullptr;

SPtr<GraphicsAPI> g_pGraphicsAPI;
SPtr<VertexShader> g_pVertexShader;
SPtr<PixelShader> g_pPixelShader;

Texture g_dsReflection;
Texture g_rtReflection;

ID3D11InputLayout* g_pInputLayout = nullptr;

ID3D11RasterizerState1* g_pRS_Default = nullptr;
ID3D11RasterizerState1* g_pRS_Wireframe = nullptr;
ID3D11RasterizerState1* g_pRS_No_Cull = nullptr;

ID3D11SamplerState* g_pSS_Point = nullptr;
ID3D11SamplerState* g_pSS_Linear = nullptr;
ID3D11SamplerState* g_pSS_Anisotropic = nullptr;

SPtr<GraphicsBuffer> g_pCB_WVP;
MatrixCollection g_WVP;

SPtr<Camera> g_pCamera;
Vector3 g_CameraMove = { 0.0f,0.0f,0.0f };

SPtr<AudioAPI> g_pAudioAPI;
SPtr<Master> g_pMaster;
SPtr<Audio> g_pSound;
SPtr<Audio> g_pSound2;
SPtr<Submix> g_pSubmix;
SPtr<VoiceCallback> g_pCallback;

static float g_cameraMovSpeed = 0.001f;
static float g_cameraRotSpeed = .01f;

SPtr<World> g_pWorld;

SPtr<Character> g_pCharacter;
SPtr<Character> g_pDinosaur;
SPtr<Actor> g_pMainActor;

void Update(float deltaTime) 
{
	//g_Camera.MoveCamera(g_WVP, g_cameraMovSpeed * deltaTime);
	//g_pAudioAPI->SetCameraListener(g_pCamera);

	/*printf(	"\rCam Position: X:%f Y:%f Z:%f" ,
					g_Camera.getViewMatrix().m[3][0],
					g_Camera.getViewMatrix().m[3][1],
					g_Camera.getViewMatrix().m[3][2]);*/
	

	/*printf(	"\rPosition: X:%f Y:%f Z:%f", 
					g_pCamera->getEyePosition().x,
					g_pCamera->getEyePosition().y,
					g_pCamera->getEyePosition().z);*/

	/*printf(	"\rPosition: X:%f Y:%f Z:%f",
					g_Camera.getEyePosition().x,
					g_Camera.getEyePosition().y,
					g_Camera.getEyePosition().z);*/
	g_pCamera->Move(g_CameraMove * deltaTime);
	//g_pCamera->Rotate(30.0f * deltaTime, 30.0f * deltaTime);
	g_pCamera->Update();
	g_pWorld->Update(deltaTime);
}
void Render() {

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

	FloatColor tempColor = Color{ 0, 0, 0, 255 };
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

	g_pWorld->Render();

	g_pGraphicsAPI->m_pSwapChain->Present(0, 0);
}

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{

#pragma region SDL3_SETUP
	//---------------SDL SET UP!!-----------------
	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
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
		g_pGraphicsAPI = make_shared<GraphicsAPI>(pWndHandle);
		if(!g_pGraphicsAPI)
		{
			return SDL_APP_FAILURE;
		}
	}

	SDL_ShowCursor();
	SDL_SetWindowRelativeMouseMode(g_pWindow,true);  // optional but better
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
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,	0,  0,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR"	  , 0, DXGI_FORMAT_R32G32B32_FLOAT,	0, 12,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL"  , 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT   ,	0, 36,	D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	//---------------INPUT LAYOUT!---------------
	g_pInputLayout = g_pGraphicsAPI->CreateInputLayout(	inputElementDesc, 
																											g_pVertexShader);
	if (!g_pInputLayout)
	{
		return SDL_APP_FAILURE;
	}

	g_pCamera = make_shared<Camera>();

	//---------------CAMERA SET UP---------------
	g_pCamera->SetLookAt(Vector3(0, 0, -6.0f), Vector3(0, 0, 0), Vector3(0, 1, 0));		//Setea la matriz de la cámara
	g_pCamera->SetPerspective(3.141592653f/4.0f,WIDTH,HEIGHT,0.1f,100.0f);			//Sete la matriz de la perspectiva

	//---------------WORLD SETUP---------------
	g_WVP.world.Identity();															//Matriz de mundo se hace identidad
	g_WVP.view = g_pCamera->getViewMatrix();											//Matriz de vista se obtiene de cámara
	g_WVP.projection = g_pCamera->getProjectionMatrix();								//Matriz de proyección se obtiene de cámara
	
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

	g_pWorld = make_shared<World>();
	g_pWorld->Init();

	g_pMainActor = g_pWorld->SpawnActor<Character>(	nullptr,
																									g_pGraphicsAPI, 
																									g_WVP, 
																									g_pCamera, 
																									g_pCB_WVP, 
																									"rex_norm.obj", 
																									"Rex_C.bmp", 
																									Vector3(0.0f, -1.0f, 0.0f));

	g_pAudioAPI = make_shared<AudioAPI>(pWndHandle);
	if (!g_pAudioAPI)
	{
		return SDL_APP_FAILURE;
	}
	
	g_pAudioAPI->Init();

	g_pCallback = make_shared<VoiceCallback>();

	g_pMaster = g_pAudioAPI->CreateMaster();

	g_pSubmix = g_pAudioAPI->CreateSubmix(2, 48000);
	
	g_pSound = g_pAudioAPI->CreateSoundEffect("Mark",
																						"MX_Menu_Loop.wav", g_pCallback);
	
	
	
	g_pSound2 = g_pAudioAPI->CreateSoundEffect("Woosh",
																						"Audio_001.wav");

	g_pSound->RouteTo(g_pSubmix);
	g_pSound2->RouteTo(g_pSubmix);

	g_pSound->getSourceVoice()->SetFrequencyRatio(1.2f);
	
	g_pAudioAPI->Play(g_pSound, 1.0f);
	g_pAudioAPI->Play(g_pSound2, 1.0f);

	g_pSubmix->getSubmixVoice()->SetVolume(0.0f);
	

	int32_t cursorData[2] = { 0, 0 };
	g_pCursor = SDL_CreateCursor(	(Uint8*)cursorData, 
																(Uint8*)cursorData, 
																8, 8, 4, 4);
	SDL_SetCursor(g_pCursor);
	
	
	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
	SDL_Keycode sym = event->key.key;
	
	switch (event->type)
	{
		case SDL_EVENT_QUIT:
			return SDL_APP_SUCCESS;
			break;

		case SDL_EVENT_KEY_DOWN:
			if (sym == SDLK_W) g_CameraMove.z = g_cameraMovSpeed;
			if (sym == SDLK_S) g_CameraMove.z = -g_cameraMovSpeed;
			if (sym == SDLK_D) g_CameraMove.x = g_cameraMovSpeed;
			if (sym == SDLK_A) g_CameraMove.x = -g_cameraMovSpeed;
			if (sym == SDLK_Q) g_CameraMove.y = -g_cameraMovSpeed;
			if (sym == SDLK_E) g_CameraMove.y = g_cameraMovSpeed;
			break;
		
		case SDL_EVENT_KEY_UP:
			if (sym == SDLK_W) g_CameraMove.z = 0;
			if (sym == SDLK_S) g_CameraMove.z = 0;
			if (sym == SDLK_D) g_CameraMove.x = 0;
			if (sym == SDLK_A) g_CameraMove.x = 0;
			if (sym == SDLK_Q) g_CameraMove.y = 0;
			if (sym == SDLK_E) g_CameraMove.y = 0;
			break;

		case SDL_EVENT_MOUSE_MOTION:
		
				g_pCamera->Rotate(event->motion.xrel * g_pCamera->getRotSpeed(),
													-event->motion.yrel * g_pCamera->getRotSpeed());
				printf("X: %f", event->motion.xrel);
				printf("\tY: %f\n", event->motion.yrel);
				
			break;

	}

	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate)
{
	static Uint64 accu = 0;
	static Uint64 last = 0;
	static Uint64 past = 0;
	Uint64 now = SDL_GetTicksNS();
	float dt = (now - past) / 999999999.0f * 1000.0f;
	
	//printf("Delta Time: %f\n", dt);
	
	Update(dt);
	Render();

	if (now - last > 999999999)
	{
		last = now;
		
		accu = 0;
	}

	past = now;
	accu += 1;
	
	Uint64 elapsed = SDL_GetTicksNS() - now;
	if (elapsed < 999999) {
		SDL_DelayNS(999999 - elapsed);
	}

	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{

	if (g_pWindow)
	{
		SDL_DestroyWindow(g_pWindow);
		g_pWindow = nullptr;
	}

	SDL_DestroyCursor(g_pCursor);
	SDL_Quit();
}

