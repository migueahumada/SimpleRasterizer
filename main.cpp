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

#include "Renderer.h"

using std::function;

SDL_Window* g_pWindow = nullptr;
SDL_Cursor* g_pCursor = nullptr;

SPtr<GraphicsAPI> g_pGraphicsAPI;

SPtr<Camera> g_pCamera;
Vector3 g_CameraMove = { 0.0f,0.0f,0.0f };

SPtr<AudioAPI> g_pAudioAPI;
SPtr<Master> g_pMaster;
SPtr<Audio> g_pSound;
SPtr<Audio> g_pSound2;
SPtr<Submix> g_pSubmix;
SPtr<VoiceCallback> g_pCallback;

static float g_cameraMovSpeed = 0.001f;
static float g_cameraRotSpeed = 20.0f;

SPtr<World> g_pWorld;

SPtr<Character> g_pCharacter;
SPtr<Character> g_pDinosaur;
SPtr<Actor> g_pMainActor;

SPtr<Renderer> g_pRenderer;

void Update(float deltaTime) 
{
	g_pCamera->Move(g_CameraMove * deltaTime);
	g_pCamera->Update();
	g_pWorld->Update(deltaTime);
}

void Render() {

	g_pWorld->Render();

	g_pRenderer->SetGeometryPass();
	g_pRenderer->SetLightingPass();
	
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
	g_pCamera = make_shared<Camera>();
	g_pWorld = make_shared<World>();
	g_pRenderer = make_shared<Renderer>(g_pGraphicsAPI,g_pCamera,g_pWorld);

	


	SDL_ShowCursor();
	SDL_SetWindowRelativeMouseMode(g_pWindow,true);  // optional but better
#pragma endregion SDL3_SETUP

	//RecompileShaders();
	g_pRenderer->CompileShaders();

	g_pRenderer->InitInputLayout();

	

  /*static float tempo = 2.0f;
	tempo += 0.0001f;*/

	//---------------CAMERA SET UP---------------
	g_pCamera->SetLookAt(Vector3(0, 0, -6.0f), Vector3(0, 0, 0), Vector3(0, 1, 0));		//Setea la matriz de la cámara
	g_pCamera->SetPerspective(3.141592653f/4.0f,WIDTH,HEIGHT,0.1f,100.0f);			//Sete la matriz de la perspectiva

	g_pRenderer->InitWVP();
	g_pRenderer->InitConstantBuffer();
	g_pRenderer->InitRasterizerStates();
	g_pRenderer->InitSampleFilters();
	g_pRenderer->InitGBuffer(WIDTH, HEIGHT);
	
	g_pWorld->Init();

	g_pMainActor = g_pWorld->SpawnActor<Character>(	nullptr,
																									g_pGraphicsAPI,
																									"rex_norm.obj", 
																									"Rex_C.bmp", 
																									Vector3(0.0f, 0.0f, 0.0f),
																									"Rex_N.bmp",
																									"Rex_R.bmp",
																									"Rex_M.bmp");

	

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

			if (sym == SDLK_UP) g_pCamera->Rotate(0.0f, 2.0f);
			if (sym == SDLK_DOWN) g_pCamera->Rotate(0.0f, -2.0f);
			if (sym == SDLK_LEFT) g_pCamera->Rotate(2.0f, 0.0f);
			if (sym == SDLK_RIGHT) g_pCamera->Rotate(-2.0f, 0.0f);
			break;
		
		case SDL_EVENT_KEY_UP:
			if (sym == SDLK_W) g_CameraMove.z = 0;
			if (sym == SDLK_S) g_CameraMove.z = 0;
			if (sym == SDLK_D) g_CameraMove.x = 0;
			if (sym == SDLK_A) g_CameraMove.x = 0;
			if (sym == SDLK_Q) g_CameraMove.y = 0;
			if (sym == SDLK_E) g_CameraMove.y = 0;

			if (sym == SDLK_F2)
			{
				//RecompileShaders();
			}
			break;

		case SDL_EVENT_MOUSE_MOTION:
				g_pCamera->Rotate(event->motion.xrel * g_pCamera->getRotSpeed(), 
													event->motion.yrel * g_pCamera->getRotSpeed());
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

