#define SDL_MAIN_USE_CALLBACKS 1

#define WIDTH 1280
#define HEIGHT 720

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_keyboard.h>
#include <iostream>
#include "MathObjects.h"
#include "Texture.h"
#include "Camera.h"
#include "GraphicsAPI.h"
#include "AudioAPI.h"
#include "Character.h"
#include "World.h"
#include "Actor.h"
#include "VoiceCallback.h"
#include "Renderer.h"
#include "ImGuiAPI.h"
#include "Model.h"
#include "ResourceManager.h"
#include "SoundEngine.h"
#include "UUID.h"


SDL_Window* g_pWindow = nullptr;
SDL_Cursor* g_pCursor = nullptr;

//SPtr<GraphicsAPI> g_pGraphicsAPI;
SPtr<Camera> g_pCamera;
Vector3 g_CameraMove = { 0.0f,0.0f,0.0f };

SPtr<Master> g_pMaster;
SPtr<Audio> g_pSound;
SPtr<Audio> g_pSound2;
SPtr<Submix> g_pSubmix;
SPtr<VoiceCallback> g_pCallback;

static float g_cameraMovSpeed = 2.0f;
static float g_cameraRotSpeed = 20.0f;

SPtr<World> g_pWorld;
SPtr<Character> g_pCharacter;
SPtr<Character> g_pDinosaur;
SPtr<Actor> g_pMainActor;
SPtr<Actor> g_pSecondaryActor;
SPtr<Actor> g_pThirdActor;
SPtr<Actor> g_pFourthActor;
SPtr<Actor> g_pFifthActor;
SPtr<Actor> g_pSixthActor;
SPtr<Actor> g_pSeventhActor;


constexpr float fixedDeltaTime = 1.0f/60.0f;

void Update(float deltaTime) 
{
	g_pCamera->Move(g_CameraMove * deltaTime);
	g_pCamera->Update();
	g_pWorld->Update(deltaTime);
	g_imguiAPI().Update();
	g_soundEngine().Update();
}

void FixedUpdate(){
	g_pWorld->FixedUpdate();
	//printf("FixedUpdateLlamado\n");
}

void Render() {

	g_pWorld->Render();

	g_renderer().SetPasses();

	g_imguiAPI().Render();
	
	g_graphicsAPI().m_pSwapChain->Present(0, 0);
}

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{

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
		GraphicsAPI::StartUp(pWndHandle);
	}

	g_pCamera = make_shared<Camera>();
	g_pWorld = make_shared<World>();

	Renderer::StartUp(g_pCamera, g_pWorld);
	AudioAPI::StartUp();
	SoundEngine::StartUp();
	ResourceManager::StartUp();
	ImGuiAPI::StartUp(g_pWindow, g_pWorld, g_pCamera);

	g_imguiAPI().Init(g_graphicsAPI().m_pDevice, g_graphicsAPI().m_pDeviceContext);

	SDL_ShowCursor();

	g_renderer().CompileShaders();

	g_renderer().InitInputLayout();

	g_pCamera->SetLookAt(Vector3(0, 0, -6.0f), Vector3(0, 0, 0), Vector3(0, 1, 0));		//Setea la matriz de la cámara
	g_pCamera->SetPerspective(3.141592653f/4.0f,WIDTH,HEIGHT,0.1f,100.0f);			//Sete la matriz de la perspectiva

	g_renderer().InitWVP();
	g_renderer().InitConstantBuffer();
	g_renderer().InitRasterizerStates();
	g_renderer().InitSampleFilters();
	g_renderer().InitGBuffer(WIDTH, HEIGHT);
	g_renderer().SetDefaultTextures();
	
	g_pWorld->Init();

	g_pFourthActor = g_pWorld->SpawnActor<Character>(nullptr,
		"D:/Coding/C++/SimpleRasterizer/Models/DiscFloor/Disc.obj",
		Vector3(0.0f, 0.0f, 0.0f),
		Vector3(4.0f, 4.0f, 4.0f));
	g_pFourthActor->SetName("Floor");

	g_pSeventhActor = g_pWorld->SpawnActor<Character>(nullptr,
		"D:/Models3D/RexTestModel/Rex_mat.obj",
		Vector3(0.0f, 0.0f, 0.0f),
		Vector3(1.0f, 1.0f, 1.0f));

	g_pSeventhActor->SetName("REX");

	
	
	g_pSecondaryActor = g_pWorld->SpawnActor<Character>(nullptr,
																											"D:/Models3D/brr_brr_patapim_game_ready_3d_model_free/BrainRot.obj",
																											Vector3(6.0f, 0.0f, 0.0f),
																											Vector3(1.0f, 1.0f, 1.0f));
	g_pSecondaryActor->SetName("Patapim");


	g_pSecondaryActor = g_pWorld->SpawnActor<Character>(nullptr,
																											"D:/Models3D/PlayStation 2 - Silent Hill 2 - Pyramid Head/PyramidHead.obj",
																											Vector3(-3.0f, 0.0f, 0.0f),
																											Vector3(1.0f, 1.0f, 1.0f));
	g_pSecondaryActor->SetName("Pyramid Head");
	
	
	

	//g_audioAPI().Init();

	//g_pCallback = make_shared<VoiceCallback>();

	//g_pMaster = g_audioAPI().CreateMaster();

	//g_audioAPI().Init3DAudio(g_pMaster);

	//g_pSubmix = g_audioAPI().CreateSubmix(2, 48000);
	//
	//g_pSound = g_audioAPI().CreateSoundEffect("Mark",
	//																					"./audio/MX_Menu_Loop.wav", g_pCallback);
	//
	//g_pSound2 = g_audioAPI().CreateSoundEffect("Woosh",
	//																					"./audio/Audio_001.wav");

	//g_pSound->RouteTo(g_pSubmix);
	//g_pSound2->RouteTo(g_pSubmix);

	//g_pSound->getSourceVoice()->SetFrequencyRatio(1.2f);
	//g_pSubmix->getSubmixVoice()->SetVolume(1.0f);
	//g_audioAPI().Play(g_pSound, 0.8f);
	//g_audioAPI().Play(g_pSound2, 0.3f);

	

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
	
	g_imguiAPI().Input(event);

	switch (event->type)
	{
		case SDL_EVENT_QUIT:
			return SDL_APP_SUCCESS;
			break;

		case SDL_EVENT_KEY_DOWN:
			if (SDL_GetWindowRelativeMouseMode(g_pWindow))
			{
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
			}
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
				g_renderer().CompileShaders();
				//RecompileShaders();
			}
			break;

		case SDL_EVENT_MOUSE_MOTION:
			
			g_pCamera->SelectObjectOnScreen(event->motion.x, event->motion.y);
			
			//std::cout << event->motion.x << " " << event->motion.y << std::endl;

			if (g_pCamera->canRotate() && SDL_GetWindowRelativeMouseMode(g_pWindow))
			{
				g_pCamera->Rotate(event->motion.xrel * g_pCamera->getRotSpeed(),
					event->motion.yrel * g_pCamera->getRotSpeed());
			}
			
			break;

		case SDL_EVENT_MOUSE_BUTTON_DOWN:
			g_pCamera->enableRotation();
			if (event->button.button == SDL_BUTTON_RIGHT) 
				SDL_SetWindowRelativeMouseMode(g_pWindow, true);		

			break;

		case SDL_EVENT_MOUSE_BUTTON_UP:
			g_pCamera->disableRotation();
			if (event->button.button == SDL_BUTTON_RIGHT) 
				SDL_SetWindowRelativeMouseMode(g_pWindow, false);
			
			if (event->button.button == SDL_BUTTON_LEFT)
			{
				g_renderer().Pick();

				if (g_renderer().m_hit)
				{
					std::cout << "Actor hit ray" << std::endl;
				}
				else
				{
					std::cout << "NO HIT" << std::endl;
				}

			}

			break;

	}

	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate)
{
	static Uint64 last = 0;
	static Uint64 past = 0;
	static float tiempoAcumulado = 0.0f;

	Uint64 now = SDL_GetTicksNS();
	
	if (past == 0) {
		past = now;
		return SDL_APP_CONTINUE;
	}
	
	float dt = (now - past) / 1000000000.0f;

	//printf("DeltaTime: %f\n",dt);
	
	Update(dt);

	//La cantidad de veces que fixed update va a correr es relativo a cuántas veces es mayor el dt
	
	// Caso 1: DeltaTime es mayor al fixedDeltaTime
	// Fixed |-------|-------|
	// Delta ^-----------^---^ 

	// Caso 2: DeltaTime es menor al fixedDeltaTime
	// Fixed |-------|-------|
	// Delta ^--^---^----^---^ 

	tiempoAcumulado += dt;

	while (tiempoAcumulado >= fixedDeltaTime) // si el dt es mayor al fdt
	{
		FixedUpdate();
		tiempoAcumulado -= fixedDeltaTime;
	}
	
	Render();

	if (now - last > 1000000000)
	{
		last = now;
	}

	past = now;

	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
	
	AudioAPI::Shutdown();
	SoundEngine::Shutdown();
	
	ImGuiAPI::Shutdown();
	ResourceManager::Shutdown();
	Renderer::Shutdown();
	GraphicsAPI::Shutdown();

	

	if (g_pWindow)
	{
		SDL_DestroyWindow(g_pWindow);
		g_pWindow = nullptr;
	}

	SDL_DestroyCursor(g_pCursor);
	SDL_Quit();
}

