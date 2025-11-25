#pragma once

#include "Module.h"
#include <SDL3/SDL.h>

namespace MKEngine{
  class App : public Module<App>
  {
  public:
    App();
    virtual ~App();

    void OnStartUp() override;
    void OnShutdown() override;
    void runMainLoop();

  private:
    void inputs();
    void update(float deltaTime);
    void render();

  protected:
    bool m_runMainLoop;
    SDL_Window* m_pWindow;
    

    /*
    * GraphicsAPI
    * AudioAPI
    * ImGuiAPI
    * Renderer
    * Camera
    * World
    */

    const int m_width{1280};
    const int m_height{720};
  };
}

