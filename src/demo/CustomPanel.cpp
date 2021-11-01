#include "CustomPanel.h"

#include "Context.h"
#include "Debug.h"

using namespace std;

CustomPanel::CustomPanel()
{
  debug::analyser.type_constructed(this);
}

std::shared_ptr<CustomPanel> CustomPanel::get_instance()
{
  return std::shared_ptr<CustomPanel>(new CustomPanel());
}

CustomPanel::~CustomPanel()
{
  if (fps_texture) {
    SDL_DestroyTexture(fps_texture);
  }
  if (name_texture) {
    SDL_DestroyTexture(fps_texture);
  }
  if (status_texture) {
    SDL_DestroyTexture(fps_texture);
  }
  debug::analyser.type_deconstructed(this);
}

void CustomPanel::on_event(const SDL_Event &event)
{
  // TODO: handle events like mouse movement and clicks for buttons in panel
  // e.t.c.
}

void CustomPanel::paint(SDL_Renderer *renderer, shared_ptr<Context> context)
{
  shared_ptr<Image> image = context->get_resource_manager()->get_image("tiles.png");
  paint_life_bar(renderer, image); // No functionality implemented yet.
  paint_status(renderer, context);
  // paint_full_screen_button(renderer, image); // Works, but have no
  // functionality.
}

void CustomPanel::paint_life_bar(SDL_Renderer *renderer, shared_ptr<Image> image)
{
  // So ugly. We should do this properly -- like this whole class. Its a quick
  // dirty made panel for demonstration.
  SDL_Rect filled_heart_src = {0, 96, 32, 32};
  SDL_Rect empty_heart_src = {32, 96, 32, 32};

  SDL_Rect dst = {(64 - 32) / 2, (64 - 32) / 2, 32, 32};
  for (int i = 0; i < 4; ++i) {
    SDL_RenderCopy(renderer, image->get_texture(), &filled_heart_src, &dst);
    dst.x += 32;
  }
}

void CustomPanel::paint_full_screen_button(SDL_Renderer *renderer, shared_ptr<Image> image)
{
  SDL_Rect full_screen_src = {0, 160, 64, 64};
  SDL_Rect window_screen_src = {64, 160, 64, 64};

  SDL_Rect dst = {308, 8, 48, 48};
  SDL_SetRenderDrawColor(renderer, 24, 24, 24, 255);
  SDL_RenderFillRect(renderer, &dst);
  SDL_SetRenderDrawColor(renderer, 185, 0, 0, 255);
  SDL_RenderDrawRect(renderer, &dst);
  dst = {300, 0, 64, 64};
  SDL_RenderCopy(renderer, image->get_texture(), &full_screen_src, &dst);
}

void CustomPanel::paint_edit_button(SDL_Renderer *renderer, shared_ptr<Image> image)
{
  // TODO: implement this.
}

// TODO: redo all, its copy paste, large and not pretty.
void CustomPanel::paint_status(SDL_Renderer *renderer, std::shared_ptr<Context> context)
{
  shared_ptr<ResourceManager> resource_manager = context->get_resource_manager();
  const SDL_Rect &map_view = context->get_map_manager()->current_map()->get_map_dimension();

  int new_is_full_screen = context->get_int_data("FULL_SCREEN_MODE", 0);
  bool redraw = full_screen_mode != new_is_full_screen;

  int new_fps = context->get_int_data("FPS", 0);
  if (fps != new_fps || redraw) {
    fps = new_fps;
    if (fps_texture) {
      SDL_DestroyTexture(fps_texture);
    }

    string fps_str = "FPS: " + to_string(fps);
    TTF_Font *large_font = resource_manager->get_or_load_font("clacon.ttf", 48);
    int w, h;
    TTF_SizeText(large_font, fps_str.c_str(), &w, &h);
    fps_rectangle = {map_view.w - w, 0, w, h};
    fps_texture = paint_text(renderer, large_font, fps_str, {255, 255, 255, 255});
  }

  string new_name = context->get_string_data("NAME", "Little Green Fella");
  if (name != new_name || redraw) {
    name = new_name;
    if (name_texture) {
      SDL_DestroyTexture(name_texture);
    }

    TTF_Font *tiny_font = resource_manager->get_or_load_font("clacon.ttf", 24);
    int w, h;
    TTF_SizeText(tiny_font, name.c_str(), &w, &h);
    name_rectangle = {0, map_view.h - h - 15, w, h}; // Ugly hard coded indentation.
    name_texture = paint_text(renderer, tiny_font, name, {255, 255, 255, 255});
  }

  if (redraw) {
    full_screen_mode = new_is_full_screen;
    if (status_texture) {
      SDL_DestroyTexture(status_texture);
    }

    string on_off = full_screen_mode == 0 ? "OFF" : "ON";
    string status_str = "Press -> [A]: Shoot (pixel collision), "
                        "[W]: Write name, "
                        "[CTRL-F]: Full screen [" +
                        on_off +
                        "], "
                        "[ESC]: Quit dialog";
    TTF_Font *tiny_font = resource_manager->get_or_load_font("clacon.ttf", 18);
    int w, h;
    TTF_SizeText(tiny_font, status_str.c_str(), &w, &h);
    status_rectangle = {0, map_view.h - h, w, h};
    status_texture = paint_text(renderer, tiny_font, status_str, {255, 255, 255, 255});
  }

  SDL_RenderCopy(renderer, fps_texture, NULL, &fps_rectangle);
  SDL_RenderCopy(renderer, name_texture, NULL, &name_rectangle);
  SDL_RenderCopy(renderer, status_texture, NULL, &status_rectangle);
}

SDL_Texture *CustomPanel::paint_text(SDL_Renderer *renderer, TTF_Font *font, string caption,
                                     const SDL_Color &color)
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
  SDL_FreeSurface(font_surface);
  return texture;
}
