#include "TextBox.h"

#include "Debug.h"
#include "Utility.h"
#include <exception>

using namespace std;

TextBox::TextBox(int w, int h, int max_characters, const string &init_text)
    : UIComponent(-1, -1, w, h), max_characters(max_characters), text(init_text), timer()
{
  debug::analyser.type_constructed(this);
}

TextBox::~TextBox()
{
  SDL_StopTextInput();
  SDL_DestroyTexture(texture);
  SDL_DestroyTexture(texture_focused);
  debug::analyser.type_deconstructed(this);
}

std::shared_ptr<TextBox> TextBox::get_instance(int w, int h, int max_characters,
                                               const string &init_text)
{
  return std::shared_ptr<TextBox>(new TextBox(w, h, max_characters, init_text));
}

void TextBox::initialize(shared_ptr<Context> context)
{
  SDL_Renderer *renderer = context->get_window()->get_renderer_ptr();
  texture = SDL_CreateTexture(renderer, 0, SDL_TEXTUREACCESS_TARGET, dimension.w, dimension.h);
  utility::fill_and_outline(renderer, texture, 1, {0xFF, 0xFF, 0xFF, 0xFF},
                            {0x00, 0x00, 0x00, 0xFF});
  texture_focused =
      SDL_CreateTexture(renderer, 0, SDL_TEXTUREACCESS_TARGET, dimension.w, dimension.h);
  utility::fill_and_outline(renderer, texture_focused, 1, {0xFF, 0xFF, 0xFF, 0xFF},
                            {0xFF, 0x00, 0x00, 0xFF});

  // TODO: we probably want to start the text input when a user have clicked in
  // the text box,
  // TODO: when that is done the text cursor should start blinking and the text
  // input activated.
  SDL_StartTextInput();
}

int renderText(SDL_Renderer *renderer, SDL_Rect rect, string text)
{
  if (text.size() > 0) {
    // TODO: refactor, we want to load fonts from resource manager.
    string font_path = "joystix.ttf";
    TTF_Font *font = TTF_OpenFont(font_path.c_str(), 10);
    if (font == nullptr) {
      debug::runtime_error(string("TTF_OpenFont: could not open font " + font_path));
    }

    const char *str = text.c_str();
    SDL_Color font_color = {0, 0, 0, 255};
    SDL_Surface *font_surface = TTF_RenderText_Solid(font, str, font_color);
    if (font_surface == nullptr) {
      TTF_CloseFont(font);
      debug::runtime_error("TTF_RenderText_Blended: could render font.");
    }
    int font_width = font_surface->w;

    SDL_Texture *font_texture = SDL_CreateTextureFromSurface(renderer, font_surface);
    if (font_texture == nullptr) {
      TTF_CloseFont(font);
      SDL_FreeSurface(font_surface);
      debug::runtime_error("SDL_Create_TextureFromSurface: could not create texture.");
    }

    SDL_Rect dst = {rect.x + 4, rect.y + (rect.h - font_surface->h) / 2, font_surface->w,
                    font_surface->h};
    SDL_RenderCopy(renderer, font_texture, NULL, &dst);

    SDL_FreeSurface(font_surface);
    SDL_DestroyTexture(font_texture);
    return rect.x + font_width;
  }
  return rect.x;
}

void TextBox::paint(SDL_Renderer *renderer, const SDL_Rect &clip)
{
  SDL_Texture *t = is_focused ? texture_focused : texture;

  SDL_RenderCopy(renderer, t, NULL, &dimension);
  int x_pos = renderText(renderer, dimension, text);

  if (timer.is_obselete(750)) {
    flag *= -1;
  }

  if (flag > 0) {
    // Render a text cursor.
    SDL_Rect r = {x_pos + 4, dimension.y + 2, 2, dimension.h - 4};
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, &r);
  }
}

void TextBox::on_event(const SDL_Event &e, std::unordered_map<std::string, std::string> &event_data)
{
  if (e.type == SDL_MOUSEMOTION) {
    int mx = e.motion.x;
    int my = e.motion.y;
    is_focused = mx > dimension.x && mx < dimension.x + dimension.w && my > dimension.y &&
                 my < dimension.y + dimension.h;
  } else if (is_focused && e.type == SDL_MOUSEBUTTONDOWN) {
    is_pressed = true;
    // TODO: implement something here?
  } else if (is_focused && e.type == SDL_MOUSEBUTTONUP) {
    is_pressed = false;
    // TODO: implement something here?
  } else if (e.type == SDL_TEXTINPUT) {
    if (text.size() < max_characters) {
      text.append(e.text.text); // Append character.
      event_data[EVENT_INPUT_TEXT] = text;
    }
  } else if (e.type == SDL_KEYDOWN) {
    // Remove character.
    if (e.key.keysym.sym == SDLK_BACKSPACE && text.length() > 0) {
      text.pop_back();
      event_data[EVENT_INPUT_TEXT] = text;
    }
  }
}

const std::string &TextBox::get_caption()
{
  return text;
}
