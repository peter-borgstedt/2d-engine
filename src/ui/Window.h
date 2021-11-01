#ifndef WINDOW_H
#define WINDOW_H

#include <SDL.h>
#include <string>

#include <memory>

class Window
{
public:
  ~Window();

  static std::shared_ptr<Window> get_instance(std::string title, int x, int y, int w, int h,
                                              Uint32 window_flags = SDL_WINDOW_SHOWN);

  // Get raw pointer to SDL_Window. Do NOT destroy pointer.
  // NOTE: function is used internally by the engine classes.
  // TODO: make this class friend with all those who usage it.
  SDL_Window *get_window_ptr();

  // Get raw pointer to SDL_Window. Do NOT destroy pointer.
  // NOTE: function is used internally by the engine classes.
  SDL_Renderer *get_renderer_ptr();

  // A window may change under runtime, but usually the size is fixed. The
  // window may also change from window mode to full screen mode. The size
  // information is first retrieved from renderer, if no logical size has been
  // set retrieve size from window. The window may also be re-sizable, this will
  // give the accurate size (if changed).
  const SDL_Rect &get_logical_size();

  // Retrieve size from the actual window, the window size may change during
  // runtime.
  inline const SDL_Rect &get_actual_size()
  {
    SDL_GetWindowSize(window, &dimension.w, &dimension.h);
    return dimension;
  }

  // Set full screen. This is done logically.
  void set_full_screen(bool fullscreen);

  // Toggle from full screen mode to window mode, vice verse. Return true if in
  // full screen mode.
  bool toggle_full_screen();

  // Return if screen is in full screen mode.
  bool is_full_screen();

protected:
  Window(std::string title, int x, int y, int w, int h, Uint32 window_flags);

private:
  int original_width;
  int original_height;

  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Rect dimension;

  bool is_fullscreen = false;

  Window(const Window &) = delete;
  const Window &operator=(const Window &) = delete;
};

#endif
