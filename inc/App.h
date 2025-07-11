#pragma once
#include "Module.h"
#include <SDL3/SDL.h>

namespace MKEngine{
  class App : public Module<App>
  {
  public:
    App();
    virtual ~App();

    void onStartUp() override;
    void runMainLoop();

  private:
    void update();
    void render();

  protected:
    bool m_runMainLoop;
    SDL_Window* m_pWindow;
    SDL_Renderer* m_pRenderer;

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

