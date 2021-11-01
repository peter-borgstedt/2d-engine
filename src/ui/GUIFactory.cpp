#include "GUIFactory.h"

#include "Label.h"
#include "TextBox.h"

using namespace std;

// Anonymous namespace, preferred way in c++ for not polluting a namespace.
// In c you would make functions static inside the module.
namespace
{
  // Ref: https://wiki.libsdl.org/SDL_CreateRGBSurface
  SDL_Surface *create_surface(int w, int h)
  {
    Uint32 rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
#else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
#endif
    return SDL_CreateRGBSurface(0, w, h, 32, rmask, gmask, bmask, amask);
  }

  SDL_Rect get_center_of_window(shared_ptr<Window> window, int w, int h)
  {
    SDL_Rect dimension = window->get_logical_size();
    int x = (dimension.w - w) / 2;
    int y = (dimension.h - h) / 2;
    return {x, y, w, h};
  }

  // Do a full engine render (not really a screen shot) into a texture, this
  // will be used as a background for the dialog NOTE: we do not want to
  // transfer pixels from the screen (VM), which was the first approach i did,
  // but when toggled into a logical full screen the resolution will be the same
  // as my desktop, transferring that amount of pixels is a noticeable
  // bottleneck -- and it felt ugly. This approach is faster and feels cleaner.
  SDL_Texture *render_screen_shot(shared_ptr<Context> context)
  {
    shared_ptr<Window> window = context->get_window();
    SDL_Rect logical_size = window->get_logical_size();
    SDL_Texture *texture =
        SDL_CreateTexture(window->get_renderer_ptr(), SDL_PIXELFORMAT_RGBA8888,
                          SDL_TEXTUREACCESS_TARGET, logical_size.w, logical_size.h);
    context->get_engine_core()->paint_copy(texture, context);
    return texture;
  }
} // namespace

shared_ptr<Dialog> create_dialog(int width, int size, shared_ptr<Window> window)
{
  SDL_Rect window_center = get_center_of_window(window, width, size);
  return Dialog::get_instance(window_center.x, window_center.y, window_center.w, window_center.h);
}

shared_ptr<Dialog> GUIFactory::create_simple_option_dialog(
    shared_ptr<Context> context, int width, int height, const string &label_caption,
    const string &button1_caption, const string &button2_caption,
    const Button::action_function &button1_action, const Button::action_function &button2_action)
{
  auto window = context->get_window();
  auto dialog = create_dialog(width, height, window);

  dialog->add(0, Label::get_instance(context->get_resource_manager(), label_caption));
  dialog->add(1, Button::get_instance(100, 25, button1_caption, button1_action));
  dialog->add(1, Button::get_instance(100, 25, button2_caption, button2_action));

  dialog->initialize(context);
  return dialog;
}

shared_ptr<Dialog> GUIFactory::create_simple_input_dialog(
    shared_ptr<Context> context, int width, int height, const string &label_caption,
    const string &button1_caption, const string &button2_caption,
    const Button::action_function &button1_action, const Button::action_function &button2_action,
    const string &init_text)
{
  auto window = context->get_window();
  auto dialog = create_dialog(width, height, window);

  dialog->add(0, Label::get_instance(context->get_resource_manager(), label_caption));
  dialog->add(1, TextBox::get_instance(250, 30, 30, init_text));
  dialog->add(2, Button::get_instance(100, 25, button1_caption, button1_action));
  dialog->add(2, Button::get_instance(100, 25, button2_caption, button2_action));

  dialog->initialize(context);
  return dialog;
}

void GUIFactory::show_dialog(shared_ptr<Window> window, shared_ptr<Dialog> dialog,
                             SDL_Texture *background)
{
  SDL_Renderer *renderer = window->get_renderer_ptr();
  SDL_Rect window_area = window->get_logical_size();

  // Clear any previous events from queue.
  SDL_FlushEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);

  SDL_Event event;
  unordered_map<string, string> event_data;
  bool run = true;
  while (run) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT ||
          event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
        run = false;
        break;
      }
      dialog->on_event(event, event_data);
    }

    if (background) {
      SDL_RenderCopy(renderer, background, NULL, NULL);
    }

    dialog->paint(renderer, window_area);
    SDL_RenderPresent(renderer);
  }
}

void GUIFactory::show_shaded_dialog(std::shared_ptr<Context> context,
                                    std::shared_ptr<Dialog> dialog, SDL_Color color)
{
  SDL_Renderer *renderer = context->get_window()->get_renderer_ptr();
  SDL_Texture *texture = render_screen_shot(context);

  SDL_SetRenderTarget(renderer, texture);

  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
  SDL_RenderFillRect(renderer, NULL);

  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
  SDL_SetRenderTarget(renderer, NULL);

  show_dialog(context->get_window(), dialog, texture);

  SDL_DestroyTexture(texture); // We destroy texture here.
}
