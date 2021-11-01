#include "Window.h"
#include "Debug.h"
#include <string>

using namespace std;

Window::Window(string title, int x, int y, int w, int h, Uint32 window_flags)
    : window(SDL_CreateWindow(title.c_str(), x, y, w, h, window_flags)), dimension({0, 0, -1, -1})
{
  Uint32 render_params = SDL_SWSURFACE | SDL_RENDERER_ACCELERATED;
  // We want VSYNC on, else flickering will occur when render, this will
  // synchronize with the screen refresh rate. The down part is that this will
  // slow down the drawing, usually to 60 fps (when screen is set on 60hz).
  render_params |= SDL_RENDERER_PRESENTVSYNC;

  if (!window) {
    debug::runtime_error(string("SDL_Init Error : ").append(SDL_GetError()));
  }

  renderer = SDL_CreateRenderer(window, -1 /* use first supporting driver */, render_params);
  if (!renderer) {
    debug::runtime_error(string("SDL_CreateRenderer Error: ").append(SDL_GetError()));
  }

  debug::analyser.type_constructed(this);
}

Window::~Window()
{
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  debug::analyser.type_deconstructed(this);
}

std::shared_ptr<Window> Window::get_instance(std::string title, int x, int y, int w, int h,
                                             Uint32 window_flags)
{
  return shared_ptr<Window>(new Window(title, x, y, w, h, window_flags));
}

SDL_Window *Window::get_window_ptr()
{
  return window;
}

SDL_Renderer *Window::get_renderer_ptr()
{
  return renderer;
}

const SDL_Rect &Window::get_logical_size()
{
  // If logical a size is set retrieve it from the renderer, width and height
  // will be 0 if no logical size has been set. Ref:
  // https://wiki.libsdl.org/SDL_RenderGetLogicalSize
  SDL_RenderGetLogicalSize(renderer, &dimension.w, &dimension.h);
  // If no logical size has been set retrieve size from window.
  if (dimension.w == 0 && dimension.h == 0) {
    return get_actual_size();
  }
  return dimension;
}

void Window::set_full_screen(bool set_fullscreen)
{
  if (is_fullscreen == set_fullscreen) {
    return; // No change.
  }
  if (set_fullscreen) {
    SDL_DisplayMode dm;
    if (SDL_GetDesktopDisplayMode(0 /* screen index */, &dm) != 0) {
      debug::runtime_error("Getting desktop display mode failed: ");
    }

    /* NOTE: this is only needed when setting a logical size.
       // Calculate height from width depending on the physical screen size,
       this will be
       // different depending on screen format 16:10, 16:9 e.t.c.
       double aspect_ratio = (double)dm.h / dm.w;
       int lw;
       SDL_GetWindowSize(window, &lw, NULL);
       int lh = int(aspect_ratio*lw);

       debug::log("Screen resolution is " + to_string(dm.h) + "x" +
       to_string(dm.h)); debug::log("Screen aspect ratio is " +
       to_string(aspect_ratio)); debug::log("Determined screen size to " +
       to_string(lw) + "x" + to_string(lh));

       SDL_RenderSetLogicalSize(renderer, lw, lh);
     */

    // We can use SDL_WINDOW_FULLSCREEN_DESKTOP and set a logical size, see the
    // commented out code above. That will use the desktop resolution and resize
    // pixels. It's a bottleneck though when using a resolution. Because of this
    // I will use a full screen with the change of to an appropriate to the
    // resolution of the window. NOTE: we need to update map dimension with the
    // new window resolution for the whole window to be drawn. Also any buffered
    // textures need to be updated as well.
    debug::log("Switching on full screen.");
    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

  } else {
    debug::log("Switching off full screen.");
    SDL_SetWindowFullscreen(window, 0 /* windowed mode */);
  }
  is_fullscreen = set_fullscreen;
}

bool Window::toggle_full_screen()
{
  set_full_screen(!is_fullscreen);
  return is_fullscreen;
}

bool Window::is_full_screen()
{
  return is_fullscreen;
}
