#ifndef UI_COMPONENT_H
#define UI_COMPONENT_H

#include "Context.h"
#include <SDL.h>

class UIComponent
{
  friend class Dialog;

public:
  UIComponent(const int &x, const int &y, const int &w, const int &h);
  virtual ~UIComponent();

  const SDL_Rect &get_dimension() const;

  virtual void initialize(std::shared_ptr<Context> context){};
  virtual void paint(SDL_Renderer *renderer, const SDL_Rect &clip) = 0;
  virtual void on_event(const SDL_Event &e,
                        std::unordered_map<std::string, std::string> &event_data){};

protected:
  SDL_Rect dimension;
  SDL_Rect &get_dimension();

private:
  UIComponent(const UIComponent &) = delete;
  const UIComponent &operator=(const UIComponent &) = delete;
};

#endif
