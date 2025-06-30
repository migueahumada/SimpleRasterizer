#pragma once
#define WIN32_LEAN_AND_MEAN
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_dx11.h"
#include <Windows.h>
#include "HelperMacros.h"

class MKWindow;
class World;
class Camera;
class Renderer;

class ImGuiAPI
{
public:
	ImGuiAPI(SDL_Window* pWindow, 
					const WPtr<World>& pWorld, 
					const WPtr<Camera>& pCamera,
					const WPtr<Renderer>& pRenderer);
	
	~ImGuiAPI();

	bool Init(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	void Input(SDL_Event* event);
	void Update();
	void Render();

private:

	SDL_Window* m_pWindow;
	WPtr<World> m_pWorld;
	WPtr<Camera> m_pCamera;
	WPtr<Renderer> m_pRenderer;

};

