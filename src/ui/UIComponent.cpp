#include "UIComponent.h"
#include "Debug.h"

UIComponent::UIComponent(const int &x, const int &y, const int &w, const int &h)
    : dimension({x, y, w, h})
{
  debug::analyser.type_constructed(this);
}

UIComponent::~UIComponent()
{
  debug::analyser.type_deconstructed(this);
}

const SDL_Rect &UIComponent::get_dimension() const
{
  return dimension;
}

SDL_Rect &UIComponent::get_dimension()
{
  return dimension;
}
