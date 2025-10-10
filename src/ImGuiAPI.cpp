#include "ImGuiAPI.h"
#include "World.h"
#include "Camera.h"
#include "Renderer.h"
#include "SoundEngine.h"
#include "Channel.h"
#include "Audio.h"
#include <xaudio2.h>


ImGuiAPI::ImGuiAPI(SDL_Window* pWindow, 
									 const WPtr<World>& pWorld, 
									 const WPtr<Camera>& pCamera)
	: m_pWindow(pWindow), 
		m_pWorld(pWorld), 
		m_pCamera (pCamera) 
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
	if (m_pWorld.expired() || m_pCamera.expired())
	{
		return;
	}

	auto WORLD = m_pWorld.lock();
	auto CAMERA = m_pCamera.lock();

	ImGui::ShowDemoWindow();


	ImGui::SetNextWindowBgAlpha(0.2f);
	SetSceneGraphUI(true);


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
		ImGui::Image((ImTextureID)g_renderer().getGBuffer().at(0).m_pSRV,
									ImVec2(320, 180));
		
		ImGui::Text("Normal");
		ImGui::Separator();
		ImGui::Image((ImTextureID)g_renderer().getGBuffer().at(1).m_pSRV,
									ImVec2(320, 180));
		
		ImGui::Text("Color");
		ImGui::Separator();
		ImGui::Image((ImTextureID)g_renderer().getGBuffer().at(2).m_pSRV,
									ImVec2(320, 180));
		
		ImGui::Text("SSAO");
		ImGui::Separator();
		ImGui::Image((ImTextureID)g_renderer().getGBuffer().at(3).m_pSRV,
									ImVec2(320, 180));
		
		ImGui::Text("Shadow Map");
		ImGui::Separator();
		ImGui::Image((ImTextureID)g_renderer().getShadowMap().m_pSRV,
			ImVec2(320, 180));
	ImGui::End();

	SetSoundEngineUI(true);
	

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}

void ImGuiAPI::SetSoundEngineUI(bool bSet)
{
	if (bSet == false)
	{
		return;
	}

	ImGui::Begin("Sound Engine");
	
	if (ImGui::CollapsingHeader("Channels", ImGuiTreeNodeFlags_SpanAvailWidth))
	{
		if (ImGui::BeginTable("Active Channels", 1))
		{
			ImGui::TableSetupColumn("Channels", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableHeadersRow();

			for (auto it = g_soundEngine().GetChannels().begin(),
				itEnd = g_soundEngine().GetChannels().end();
				it != itEnd;
				++it)
			{
				ImGui::TableNextColumn();
				ImGui::Text(it->first.c_str());
			}
			ImGui::EndTable();
		}
	}
	

	
	//ImGui::Text("%f", g_soundEngine().GetAudios().find("Audio2")->second->getSample(122));
	if (ImGui::CollapsingHeader("AudioSettings",ImGuiTreeNodeFlags_SpanAvailWidth))
	{
		
		ImGuiStyle* style = &ImGui::GetStyle();
		
		ImGui::SliderFloat4("Waveform Color", reinterpret_cast<float*>(&style->Colors[ImGuiCol_PlotLines]), 0.0f, 1.0f);
		
		ImVec4 LastColor = style->Colors[ImGuiCol_FrameBg];
		
		ImGui::SliderFloat4("Frame Background", reinterpret_cast<float*>(&m_BgWaveformColor), 0.0f, 1.0f);

		style->Colors[ImGuiCol_FrameBg] = m_BgWaveformColor;
		
		ImGui::PlotLines(g_soundEngine().GetAudios().find("Audio1")->first.c_str(),
			g_soundEngine().GetAudios().find("Audio1")->second->getAmplitudeSamples().data(),
			g_soundEngine().GetAudios().find("Audio1")->second->getAmplitudeSamples().size(),
			0,
			g_soundEngine().GetAudios().find("Audio1")->first.c_str(),
			-1.0f, 1.0f, ImVec2(ImGui::GetWindowSize().x, 150.0f), 4);
			

		ImGui::PlotLines(g_soundEngine().GetAudios().find("Audio2")->first.c_str(),
			g_soundEngine().GetAudios().find("Audio2")->second->getAmplitudeSamples().data(),
			g_soundEngine().GetAudios().find("Audio2")->second->getAmplitudeSamples().size(),
			0,
			g_soundEngine().GetAudios().find("Audio2")->first.c_str(),
			-1.0f, 1.0f, ImVec2(ImGui::GetWindowSize().x, 150.0f), 4);

		ImGui::PlotLines(g_soundEngine().GetAudios().find("Audio3")->first.c_str(),
			g_soundEngine().GetAudios().find("Audio3")->second->getAmplitudeSamples().data(),
			g_soundEngine().GetAudios().find("Audio3")->second->getAmplitudeSamples().size(),
			0,
			g_soundEngine().GetAudios().find("Audio3")->first.c_str(),
			-1.0f, 1.0f, ImVec2(ImGui::GetWindowSize().x, 150.0f), 4);

		style->Colors[ImGuiCol_FrameBg] = LastColor;
	}
	
	

	ImGui::End();
}

void ImGuiAPI::SetSceneGraphUI(bool bSet)
{
	if (!bSet)
	{
		return;
	}

	if (m_pWorld.expired() || m_pCamera.expired())
	{
		return;
	}

	auto WORLD = m_pWorld.lock();
	auto CAMERA = m_pCamera.lock();

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

	ImGui::Begin("Properties");

	ImGui::Text("Directional Light");
	ImGui::DragFloat3("Light Position X: ", &g_renderer().GetWVP().lightPosition.x);

	ImGui::Text("Mouse Vector4");
	ImGui::DragFloat4("Vec4: ", &CAMERA->m_rayDir.x);

	ImGui::End();
}

void ImGuiAPI::OnShutdown()
{
}

void ImGuiAPI::OnStartUp()
{
}

ImGuiAPI& g_imguiAPI()
{
	return ImGuiAPI::GetInstance();
}
