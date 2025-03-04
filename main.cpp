#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <iostream>

#define WIDTH 1280
#define HEIGHT 720


#include "GraphicsAPI.h"

SDL_Window* g_pWindow = nullptr;

UPtr <GraphicsAPI> g_pGraphicsAPI;
UPtr<VertexShader> g_pVertexShader;
UPtr<PixelShader> g_pPixelShader;

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{

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

	g_pVertexShader = g_pGraphicsAPI->CreateVertexShader(L"BasicShader.hlsl", "vertex_main");
	if (!g_pVertexShader)
	{
		return SDL_APP_FAILURE;
	}

	g_pPixelShader = g_pGraphicsAPI->CreatePixelShader(L"BasicShader.hlsl", "pixel_main");
	if(!g_pPixelShader)
	{
		return SDL_APP_FAILURE;
	}

		
	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event * event)
{
	if (event->type == SDL_EVENT_QUIT) {
		return SDL_APP_SUCCESS;
	}
	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate)
{

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

