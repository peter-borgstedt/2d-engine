#ifndef LABEL_H
#define LABEL_H

#include <SDL_ttf.h>
#include <functional>
#include <string>

#include "UIComponent.h"

class Label : public UIComponent
{
public:
  ~Label();

  static std::shared_ptr<Label> get_instance(std::shared_ptr<ResourceManager> resource_manager,
                                             std::string caption);

  void initialize(std::shared_ptr<Context> context) override;
  void paint(SDL_Renderer *renderer, const SDL_Rect &clip) override;

protected:
  Label(std::shared_ptr<ResourceManager> resource_manager, std::string caption);

private:
  SDL_Texture *texture;
  std::string caption;
};

#endif
