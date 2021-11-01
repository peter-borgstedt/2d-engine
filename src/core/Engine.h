#ifndef S2D_ENGINE_H
#define S2D_ENGINE_H

#include <memory>
#include <string>
#include <vector>

#include <SDL.h>
#include <SDL_image.h>

#include "Context.h"
#include "Map.h"
#include "Panel.h"
#include "Window.h"

class Engine
{
public:
  ~Engine();

  static std::shared_ptr<Engine> get_instance(std::shared_ptr<Context> context);

  int run();

private:
  std::shared_ptr<Context> context;
  Engine(std::shared_ptr<Context> context);

  Engine(const Engine &) = delete;
  const Engine &operator=(const Engine &) = delete;
};

#endif
