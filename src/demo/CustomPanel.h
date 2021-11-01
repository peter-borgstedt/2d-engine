#ifndef CUSTOM_PANEL_H
#define CUSTOM_PANEL_H

// Collides with global Panel.h in MacOS framework
// TODO: all UI related should be defined in a namespace to get rid of this
#include "Panel.h"

#include "Image.h"
#include <SDL_ttf.h>

class Context;

// NOTE: A VERY hard and dirty written panel due to lack of time.
// TODO: its far from completed, make this panel pretty in both code and
// visually.
class CustomPanel : public Panel
{
public:
  static std::shared_ptr<CustomPanel> get_instance();

  ~CustomPanel();

  void on_event(const SDL_Event &event);
  void paint(SDL_Renderer *renderer, std::shared_ptr<Context> context);

  void paint_life_bar(SDL_Renderer *renderer, std::shared_ptr<Image> image);

  // TODO: implement functionality on the button, hide until then.
  void paint_full_screen_button(SDL_Renderer *renderer, std::shared_ptr<Image> image);

  // TODO: no edit button at the moment.
  void paint_edit_button(SDL_Renderer *renderer, std::shared_ptr<Image> image);

  // Temporary, just to show some state and options.
  void paint_status(SDL_Renderer *renderer, std::shared_ptr<Context> context);

  // Paint a text.
  SDL_Texture *paint_text(SDL_Renderer *renderer, TTF_Font *font, std::string caption,
                          const SDL_Color &color);

private:
  CustomPanel();

  CustomPanel(const CustomPanel &) = delete;
  const CustomPanel &operator=(const CustomPanel &) = delete;

  // Yes this is ugly, but due to lack of time I'll implement this panel dirty
  // -- and its rather code related to the game example than the engine.
  int fps = -1;
  int full_screen_mode = -1;
  std::string name = "";

  // Buffer textures.
  SDL_Rect fps_rectangle;
  SDL_Rect name_rectangle;
  SDL_Rect status_rectangle;

  SDL_Texture *fps_texture = nullptr;
  SDL_Texture *name_texture = nullptr;
  SDL_Texture *status_texture = nullptr;

  SDL_Rect bounds;
};

#endif
