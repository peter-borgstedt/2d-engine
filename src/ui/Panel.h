#ifndef PANEL_H
#define PANEL_H

#include <SDL.h>
#include <memory>

class Context;

// Abstract class (interface)
class Panel
{
public:
  virtual void on_event(const SDL_Event &event) = 0;
  virtual void paint(SDL_Renderer *renderer, std::shared_ptr<Context> context) = 0;
};

#endif
