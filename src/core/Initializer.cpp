#include "Initializer.h"
#include "Debug.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <exception>
#include <iostream>

using namespace std;

Initializer initializer; // Create a static instance.

Initializer::Initializer()
{
  debug::log("Initializing engine.\n");
  if (SDL_Init(SDL_INIT_VIDEO)) {
    throw runtime_error(string("SDL_Init error: ").append(SDL_GetError()));
  }

  if (TTF_Init() != 0) {
    throw runtime_error(string("TTF_Init error: ").append(SDL_GetError()));
  }
}

Initializer::~Initializer()
{
  debug::log("\nShutting down engine.");
  SDL_Quit();
  TTF_Quit();

  debug::analyser.print();
  std::cin.clear();
  std::cout << std::endl << "Press enter to end...";
  std::cin.ignore();
}
