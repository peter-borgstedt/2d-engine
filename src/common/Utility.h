#ifndef UTILITY_H
#define UTILITY_H

#include <functional>
#include <memory>

#include <iostream>
#include <unordered_map>
#include <utility>

#include "Camera.h"
#include "Common.h"

#include "Sprite.h"

namespace utility
{
  void move(SDL_Rect &position, const Direction &direction, const int &amount);

  inline Camera::Scope getScope(const SDL_Rect &position)
  {
    const int tile_size = 32; // Size of map sprites. TODO: get size from Map

    // If in positioned perfected in one grid x + width will generate an extra
    // grid, adding -1 will not generate one, if position is offset 1px then the
    // extra grid is taken i consideration.
    int position_left = position.x, position_right = position.x + position.w - 1;
    int position_top = position.y, position_bottom = position.y + position.h - 1;
    int x_min = position_left / tile_size, x_max = position_right / tile_size;
    int y_min = position_top / tile_size, y_max = position_bottom / tile_size;
    return {x_min, x_max, y_min, y_max};
  }

  inline void fill_and_outline(SDL_Renderer *renderer, SDL_Texture *texture, int size,
                               const SDL_Color &inner, const SDL_Color &outer)
  {
    // TODO: optimized, draw left, right, top bottom with a rectangle for each
    // parts with given size.
    int w, h;
    SDL_QueryTexture(texture, NULL, NULL, &w, &h);

    SDL_SetRenderTarget(renderer, texture);
    SDL_SetRenderDrawColor(renderer, outer.r, outer.g, outer.b, outer.a);
    // This is ugly, but SDL_RenderingDrawRect() will only outline a line size
    // of 1px.
    SDL_RenderFillRect(renderer, NULL);
    SDL_SetRenderDrawColor(renderer, inner.r, inner.g, inner.b, inner.a);
    SDL_Rect inner_rect = {size, size, w - (size * 2), h - (size * 2)};
    SDL_RenderFillRect(renderer, &inner_rect);
    SDL_SetRenderTarget(renderer, NULL);
  }
} // namespace utility

#endif
