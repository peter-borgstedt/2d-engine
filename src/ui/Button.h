#ifndef BUTTON_H
#define BUTTON_H

#include <SDL_ttf.h>
#include <functional>
#include <string>

#include "UIComponent.h"

class Button : public UIComponent
{
public:
  typedef std::function<void(const SDL_Event &,
                             const std::unordered_map<std::string, std::string> &)>
      action_function;

  ~Button();
  static std::shared_ptr<Button> get_instance(int w, int h, std::string caption,
                                              action_function action);

  void initialize(std::shared_ptr<Context> context) override;
  void paint(SDL_Renderer *renderer, const SDL_Rect &clip) override;
  void on_event(const SDL_Event &e,
                std::unordered_map<std::string, std::string> &event_data) override;

protected:
  Button(int w, int h, std::string caption, action_function action);

private:
  std::string caption;
  action_function action;

  SDL_Texture *texture;
  SDL_Texture *texture_focused;
  SDL_Texture *texture_pressed;

  bool is_focused = false;
  bool is_pressed = false;
};

#endif
