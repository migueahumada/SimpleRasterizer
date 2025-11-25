#include "App.h"
#include "MKGraphicsAPI.h"
#include "MKRenderer.h"

namespace MKEngine {
  App::App() 
    : m_runMainLoop(true), 
      m_pWindow(nullptr)
  {
  }

  App::~App()
  {
  }

  void App::OnStartUp()
  {
    //---------------SDL SET UP!!-----------------
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
      SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
    }

    m_pWindow = SDL_CreateWindow("MKEngine",
                                 m_width,
                                 m_height,
                                 SDL_WINDOW_RESIZABLE);

    auto pWndHandle = SDL_GetPointerProperty(SDL_GetWindowProperties(m_pWindow),
                                             SDL_PROP_WINDOW_WIN32_HWND_POINTER,
                                             nullptr);
    if (!pWndHandle)
    {
      return;
    }

    //Subsytems must have an order
    MKGraphicsAPI::StartUp(pWndHandle);
    MKRenderer::StartUp();

    bool isStarted = MKRenderer::isStarted();

    MKRenderer::GetInstance().CompileShaders();
    

  }

  void App::OnShutdown()
  {
    //Subsystems should be shutdown in inverse order
    MKRenderer::Shutdown();
    MKGraphicsAPI::Shutdown();

    if (m_pWindow)
    {
      SDL_DestroyWindow(m_pWindow);
      m_pWindow = nullptr;
    }

    

  }

  void App::runMainLoop()
  {
    
    while (m_runMainLoop)
    {
      static Uint64 accu = 0;
      static Uint64 last = 0;
      static Uint64 past = 0;
      Uint64 now = SDL_GetTicksNS();
      float dt = (now - past) / 999999999.0f * 1000.0f;
      inputs();
      update(dt);
      render();

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

    }
  }

  void App::inputs()
  {
    
  }

  void App::update(float deltaTime)
  {
  }

  void App::render()
  {
  }
}