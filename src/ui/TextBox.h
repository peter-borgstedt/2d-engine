#ifndef TEXT_BOX_H
#define TEXT_BOX_H

#include "Timer.h"
#include "UIComponent.h"
#include <SDL_ttf.h>
#include <functional>
#include <string>

#define EVENT_INPUT_TEXT "input_text"

class TextBox : public UIComponent
{
public:
  ~TextBox();
  static std::shared_ptr<TextBox> get_instance(int w, int h, int max_characters,
                                               const std::string &init_text = "");

  void initialize(std::shared_ptr<Context> context) override;
  void paint(SDL_Renderer *renderer, const SDL_Rect &clip) override;
  void on_event(const SDL_Event &e,
                std::unordered_map<std::string, std::string> &event_data) override;

  const std::string &get_caption();

protected:
  // For now we only handle one TextBox per dialog.
  // TODO: implement support so we cant edit several, i.e. click on TextBox to
  // activate and edit it. The event data also need to have unique keys for each
  // text box, the row and column index in the panel is a candidate key
  // [y_x_text] = "some text".
  TextBox(int w, int h, int max_charaters, const std::string &init_text);

private:
  int max_characters;
  std::string text;

  SDL_Rect textbox_position;

  SDL_Texture *texture;
  SDL_Texture *texture_focused;

  bool is_focused = false;
  bool is_pressed = false;

  int flag = 1;
  Timer timer;
};

#endif
