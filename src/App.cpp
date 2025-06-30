#include "App.h"

namespace MKEngine {
  App::App() 
    : m_runMainLoop(true), 
      m_pWindow(nullptr), 
      m_pRenderer(nullptr)
  {
  }

  App::~App()
  {
  }

  void App::onStartUp()
  {
    //---------------SDL SET UP!!-----------------
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
      SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
    }

    m_pWindow = SDL_CreateWindow("Soft Rasterizer",
                                 m_width,
                                 m_height,
                                 SDL_WINDOW_RESIZABLE);

    auto pWndHandle = SDL_GetPointerProperty(SDL_GetWindowProperties(m_pWindow),
                                             SDL_PROP_WINDOW_WIN32_HWND_POINTER,
                                             nullptr);
  }

  void App::runMainLoop()
  {
  }

  void App::update()
  {
  }

  void App::render()
  {
  }
}