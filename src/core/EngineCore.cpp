#include "EngineCore.h"
#include "Context.h"
#include "Debug.h"

using namespace std;

EngineCore::EngineCore()
{
  debug::analyser.type_constructed(this);
}

shared_ptr<EngineCore> EngineCore::get_instance()
{
  return shared_ptr<EngineCore>(new EngineCore());
}

EngineCore::~EngineCore()
{
  debug::analyser.type_deconstructed(this);
}

void EngineCore::add_view(const shared_ptr<Panel> &view)
{
  views.push_back(view);
}

void render_text(SDL_Renderer *renderer, TTF_Font *font, string caption, const SDL_Color &color,
                 const SDL_Rect &dst_rectangle)
{
  SDL_Surface *font_surface = TTF_RenderText_Blended(font, caption.c_str(), color);
  if (font_surface == nullptr) {
    debug::runtime_error("TTF_RenderText_Blended: could not render font.");
  }

  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, font_surface);
  if (texture == nullptr) {
    SDL_FreeSurface(font_surface);
    debug::runtime_error("SDL_Create_TextureFromSurface: could not create texture.");
  }
  SDL_RenderCopy(renderer, texture, NULL, &dst_rectangle);
  SDL_FreeSurface(font_surface);
  SDL_DestroyTexture(texture);
}

void EngineCore::paint(shared_ptr<Context> context)
{
  SDL_Renderer *renderer = context->get_window()->get_renderer_ptr();
  shared_ptr<Map> map = context->get_map_manager()->current_map();
  map->paint(renderer);

  // TODO: we probably want to use SDL_RenderSetViewport to for panel clip area.
  for (auto v : views)
    v->paint(renderer, context);

  SDL_RenderPresent(renderer);
}

void EngineCore::paint_copy(SDL_Texture *texture, shared_ptr<Context> context)
{
  // A bit copy paste. TODO: redo this and make this and paint more generic.
  SDL_Renderer *renderer = context->get_window()->get_renderer_ptr();
  SDL_SetRenderTarget(renderer, texture);

  shared_ptr<Map> map = context->get_map_manager()->current_map();

  // a dirty fix. TODO: do this properly.
  shared_ptr<SpriteManager> sprite_manager = map->get_sprite_manager();
  vector<shared_ptr<Sprite>> dynamic_sprites = sprite_manager->get_dynamic_sprites();
  for (auto ds : dynamic_sprites)
    sprite_manager->add_dirty_sprite(ds);
  map->paint(renderer);

  for (auto v : views)
    v->paint(renderer, context);

  SDL_SetRenderTarget(renderer, NULL); // Reset render target to default (screen).
}
