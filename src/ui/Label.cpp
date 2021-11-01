#include "Label.h"
#include "Debug.h"

using namespace std;

Label::Label(shared_ptr<ResourceManager> resource_manager, string caption)
    : UIComponent(-1, -1, -1, -1), caption(caption)
{
  // We need the size so the dialog may do the correct calculation.
  TTF_Font *font = resource_manager->get_or_load_font("clacon.ttf", 24);
  TTF_SizeText(font, caption.c_str(), &dimension.w, &dimension.h);
  debug::analyser.type_constructed(this);
}

Label::~Label()
{
  SDL_DestroyTexture(texture);
  debug::analyser.type_deconstructed(this);
}

shared_ptr<Label> Label::get_instance(shared_ptr<ResourceManager> resource_manager, string caption)
{
  return std::shared_ptr<Label>(new Label(resource_manager, caption));
}

void Label::initialize(shared_ptr<Context> context)
{
  SDL_Renderer *renderer = context->get_window()->get_renderer_ptr();

  string font_path = "clacon.ttf";
  TTF_Font *font = context->get_resource_manager()->get_or_load_font(font_path, 24);

  SDL_Color font_color = {255, 255, 255, 255};
  SDL_Surface *font_surface = TTF_RenderText_Blended(font, caption.c_str(), font_color);
  if (font_surface == nullptr) {
    debug::runtime_error("TTF_RenderText_Blended: could not render font.");
  }

  texture = SDL_CreateTextureFromSurface(renderer, font_surface);
  if (texture == nullptr) {
    SDL_FreeSurface(font_surface);
    debug::runtime_error("SDL_Create_TextureFromSurface: could not create texture.");
  }
  SDL_FreeSurface(font_surface);
}

void Label::paint(SDL_Renderer *renderer, const SDL_Rect &clip)
{
  SDL_RenderCopy(renderer, texture, NULL, &dimension);
}
