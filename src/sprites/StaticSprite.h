#ifndef STATIC_SPRITE_H
#define STATIC_SPRITE_H

#include "Sprite.h"

class StaticSprite : public Sprite
{
public:
  ~StaticSprite();
  static std::shared_ptr<StaticSprite> get_instance(std::shared_ptr<Image> image,
                                                    const SDL_Rect &src_rectangle,
                                                    const SDL_Rect &position,
                                                    const SDL_Rect &collision_offset,
                                                    int grid_layer, int grid_row, int grid_column);

protected:
  int grid_layer;
  int grid_row;
  int grid_column;

  StaticSprite(std::shared_ptr<Image> image, const SDL_Rect &src_rectangle,
               const SDL_Rect &position, const SDL_Rect &collision_offset, int grid_layer,
               int grid_row, int grid_column);
};

#endif
