#pragma once
#define SDL_MAIN_USE_CALLBACKS 0
#include "App.h"

int main(){
  
  MKEngine::App::StartUp();
  MKEngine::App::GetInstance().runMainLoop();
  MKEngine::App::Shutdown();
 
   return 0;
}