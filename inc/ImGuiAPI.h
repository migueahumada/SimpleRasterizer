#pragma once
#define WIN32_LEAN_AND_MEAN
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_dx11.h"
#include <Windows.h>
#include "HelperMacros.h"
#include "Module.h"

class MKWindow;
class World;
class Camera;
class Renderer;

class ImGuiAPI : public Module<ImGuiAPI>
{
public:
	ImGuiAPI(SDL_Window* pWindow, 
					const WPtr<World>& pWorld, 
					const WPtr<Camera>& pCamera);
	
	~ImGuiAPI();

	bool Init(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	void Input(SDL_Event* event);
	void Update();
	void Render();

private:

	void OnShutdown() override;
	void OnStartUp() override;

	SDL_Window* m_pWindow;
	WPtr<World> m_pWorld;
	WPtr<Camera> m_pCamera;
};

ImGuiAPI& g_imguiAPI();

