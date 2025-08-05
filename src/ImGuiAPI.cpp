#include "ImGuiAPI.h"
#include "World.h"
#include "Camera.h"
#include "Renderer.h"

ImGuiAPI::ImGuiAPI(SDL_Window* pWindow, 
									 const WPtr<World>& pWorld, 
									 const WPtr<Camera>& pCamera, 
									 const WPtr<Renderer>& pRenderer)
	: m_pWindow(pWindow), 
		m_pWorld(pWorld), 
		m_pCamera (pCamera), 
		m_pRenderer(pRenderer)
{

	
}

ImGuiAPI::~ImGuiAPI()
{
	ImGui_ImplDX11_Shutdown();
	//ImGui_ImplWin32_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();
}

bool ImGuiAPI::Init(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{

	
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGui::StyleColorsDark();

	if (!ImGui_ImplSDL3_InitForD3D(m_pWindow))
	{
		MessageBox(nullptr,L"Couldn't initialize imgui",L"Error",MB_OK);
		return false;
	}
	
	if (!ImGui_ImplDX11_Init(pDevice, pDeviceContext))
	{
		MessageBox(nullptr, L"Couldn't initialize imgui", L"Error", MB_OK);
		return false;
	}

	//ImGui::StyleColorsClassic();

	return true;
}

void ImGuiAPI::Input(SDL_Event* event)
{
  ImGui_ImplSDL3_ProcessEvent(event);
}

void ImGuiAPI::Update()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();
}

void ImGuiAPI::Render()
{
	if (m_pWorld.expired() || m_pCamera.expired() || m_pRenderer.expired())
	{
		return;
	}

	auto WORLD = m_pWorld.lock();
	auto CAMERA = m_pCamera.lock();
	auto RENDERER = m_pRenderer.lock();

	ImGui::ShowDemoWindow();

	ImGui::SetNextWindowBgAlpha(0.2f);
	ImGui::Begin("Scene Graph");
	if (ImGui::TreeNode("Scene"))
	{
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);

		for (const auto& actor : WORLD->getActors())
		{
			ImGui::Text(actor->GetName().c_str());
		}
		ImGui::TreePop();
	}
	ImGui::End();

	ImGui::SetNextWindowBgAlpha(0.2f);
	ImGui::Begin("Camera", (bool*)1, ImGuiWindowFlags_AlwaysAutoResize);
		
		ImGui::Text("EyePos");
		ImGui::Separator();
		ImGui::Text("X: %f , Y: %f, Z: %f",
			CAMERA->getEyePosition().x,
			CAMERA->getEyePosition().y,
			CAMERA->getEyePosition().z);
		
		
		ImGui::Text("Target Pos");
		ImGui::Separator();
		ImGui::Text("X: %f , Y: %f, Z: %f",
			CAMERA->getTargetPosition().x,
			CAMERA->getTargetPosition().y,
			CAMERA->getTargetPosition().z);

		ImGui::Text("Right Dir");
		ImGui::Separator();
		ImGui::Text("X: %f , Y: %f, Z: %f",
			CAMERA->getRightDir().x,
			CAMERA->getRightDir().y,
			CAMERA->getRightDir().z);

		ImGui::Text("Up Dir");
		ImGui::Separator();
		ImGui::Text("X: %f , Y: %f, Z: %f",
			CAMERA->getUpDir().x,
			CAMERA->getUpDir().y,
			CAMERA->getUpDir().z);

	ImGui::End();


	
	//-------Shaders----UI
	ImGui::Begin("Shaders", (bool*)1);
		//ImGui::BeginCombo("Shaders",);
	ImGui::End();


  ImGui::SetNextWindowBgAlpha(1.0f);
	ImGui::Begin("GBuffer Images", (bool*)1);
		
		ImGui::Text("Position");
		ImGui::Separator();
		ImGui::Image((ImTextureID)RENDERER->getGBuffer().at(0).m_pSRV,
									ImVec2(320, 180));
		
		ImGui::Text("Normal");
		ImGui::Separator();
		ImGui::Image((ImTextureID)RENDERER->getGBuffer().at(1).m_pSRV,
									ImVec2(320, 180));
		
		ImGui::Text("Color");
		ImGui::Separator();
		ImGui::Image((ImTextureID)RENDERER->getGBuffer().at(2).m_pSRV,
									ImVec2(320, 180));
		
		ImGui::Text("SSAO");
		ImGui::Separator();
		ImGui::Image((ImTextureID)RENDERER->getGBuffer().at(3).m_pSRV,
									ImVec2(320, 180));
		
		ImGui::Text("Shadow Map");
		ImGui::Separator();
		ImGui::Image((ImTextureID)RENDERER->getShadowMap().m_pSRV,
			ImVec2(320, 180));
	ImGui::End();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}
