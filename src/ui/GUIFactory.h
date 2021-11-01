#ifndef GUI_FACTORY_H
#define GUI_FACTORY_H

#include <SDL.h>
#include <functional>
#include <memory>

#include "Button.h"
#include "Context.h"
#include "Dialog.h"
#include "Window.h"

// This is like Java's JOptionPane but for this game engine -- but a lot smaller
// NOTE: its easy to create your own dialogs, these are just some template
// creations.
namespace GUIFactory
{
  // Creates a predefined panel with a label and two buttons.
  std::shared_ptr<Dialog> create_simple_option_dialog(
      std::shared_ptr<Context> context, int width, int height, const std::string &label_caption,
      const std::string &button1_caption, const std::string &button2_caption,
      const Button::action_function &button1_action, const Button::action_function &button2_action);

  // Creates a predefined panel with a label, text box and two buttons.
  std::shared_ptr<Dialog> create_simple_input_dialog(std::shared_ptr<Context> context, int width,
                                                     int height, const std::string &label_caption,
                                                     const std::string &button1_caption,
                                                     const std::string &button2_caption,
                                                     const Button::action_function &button1_action,
                                                     const Button::action_function &button2_action,
                                                     const std::string &init_text = "");

  // Event loop for the dialog.
  void show_dialog(std::shared_ptr<Window> window, std::shared_ptr<Dialog> dialog,
                   SDL_Texture *background = NULL);

  // Creates a texture of the current game screen and mixes it with a
  // transparent color. It will give a nice background when displaying the
  // dialog.
  void show_shaded_dialog(std::shared_ptr<Context> context, std::shared_ptr<Dialog> dialog,
                          SDL_Color color);

}; // namespace GUIFactory

#endif
