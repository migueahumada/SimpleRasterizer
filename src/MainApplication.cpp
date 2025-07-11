#pragma once
#define SDL_MAIN_USE_CALLBACKS 0
#include "App.h"

int main(){
  
  MKEngine::App::startUp();
  MKEngine::App::instance().runMainLoop();
  MKEngine::App::shutDown();
 
   return 0;
}