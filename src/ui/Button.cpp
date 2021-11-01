#include "Button.h"
#include "Debug.h"
#include "Utility.h"

using namespace std;

Button::Button(int w, int h, string caption, action_function action)
    : UIComponent(-1, -1, w, h), caption(caption), action(action)
{
  debug::analyser.type_constructed(this);
}

Button::~Button()
{
  SDL_DestroyTexture(texture);
  SDL_DestroyTexture(texture_focused);
  SDL_DestroyTexture(texture_pressed);
  debug::analyser.type_deconstructed(this);
}

shared_ptr<Button> Button::get_instance(int w, int h, std::string caption, action_function action)
{
  return shared_ptr<Button>(new Button(w, h, caption, action));
}

void Button::initialize(std::shared_ptr<Context> context)
{
  SDL_Renderer *renderer = context->get_window()->get_renderer_ptr();
  string font_path = "clacon.ttf";
  TTF_Font *font = context->get_resource_manager()->get_or_load_font(font_path, 24);

  SDL_Color font_color = {255, 255, 255, 255};
  SDL_Surface *font_surface = TTF_RenderText_Blended(font, caption.c_str(), font_color);
  if (font_surface == nullptr) {
    debug::runtime_error("TTF_RenderText_Blended: could not render font");
  }

  SDL_Texture *font_texture = SDL_CreateTextureFromSurface(renderer, font_surface);
  if (font_texture == nullptr) {
    SDL_FreeSurface(font_surface);
    debug::runtime_error("SDL_Create_TextureFromSurface: could not create texture");
  }

  texture = SDL_CreateTexture(renderer, 0, SDL_TEXTUREACCESS_TARGET, dimension.w, dimension.h);
  utility::fill_and_outline(renderer, texture, 1, {0x44, 0x44, 0x44, 0xFF},
                            {0xFF, 0xFF, 0xFF, 0xFF});

  texture_focused =
      SDL_CreateTexture(renderer, 0, SDL_TEXTUREACCESS_TARGET, dimension.w, dimension.h);
  utility::fill_and_outline(renderer, texture_focused, 1, {0x44, 0x00, 0x00, 0xFF},
                            {0xFF, 0x00, 0x00, 0xFF});

  texture_pressed =
      SDL_CreateTexture(renderer, 0, SDL_TEXTUREACCESS_TARGET, dimension.w, dimension.h);
  utility::fill_and_outline(renderer, texture_pressed, 1, {0x00, 0x00, 0x44, 0xFF},
                            {0x00, 0x00, 0xFF, 0xFF});

  SDL_Rect clip = {(dimension.w - font_surface->w) / 2, (dimension.h - font_surface->h) / 2,
                   font_surface->w, font_surface->h};
  SDL_SetRenderTarget(renderer, texture);
  SDL_RenderCopy(renderer, font_texture, NULL, &clip);

  SDL_SetRenderTarget(renderer, texture_focused);
  SDL_RenderCopy(renderer, font_texture, NULL, &clip);

  SDL_SetRenderTarget(renderer, texture_pressed);
  SDL_RenderCopy(renderer, font_texture, NULL, &clip);

  SDL_SetRenderTarget(renderer, NULL);
  SDL_FreeSurface(font_surface);

  SDL_DestroyTexture(font_texture);
}

void Button::paint(SDL_Renderer *renderer, const SDL_Rect &clip)
{
  SDL_Texture *selected_texture;
  if (is_pressed && is_focused) {
    selected_texture = texture_pressed;
  } else if (is_focused) {
    selected_texture = texture_focused;
  } else {
    selected_texture = texture;
  }
  SDL_RenderCopy(renderer, selected_texture, NULL, &dimension);
}

void Button::on_event(const SDL_Event &e, std::unordered_map<std::string, std::string> &event_data)
{
  if (e.type == SDL_MOUSEMOTION) {
    int mx = e.motion.x;
    int my = e.motion.y;
    is_focused = mx > dimension.x && mx < dimension.x + dimension.w && my > dimension.y &&
                 my < dimension.y + dimension.h;
  } else if (is_focused && e.type == SDL_MOUSEBUTTONDOWN) {
    is_pressed = true;
  } else if (e.type == SDL_MOUSEBUTTONUP) {
    if (is_focused) {
      action(e, event_data);
    }
    is_pressed = false;
  }
}
