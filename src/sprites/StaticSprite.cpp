#include "StaticSprite.h"
#include "Debug.h"

using namespace std;

shared_ptr<StaticSprite> StaticSprite::get_instance(shared_ptr<Image> image,
                                                    const SDL_Rect &src_rectangle,
                                                    const SDL_Rect &position,
                                                    const SDL_Rect &collision_offset,
                                                    int grid_layer, int grid_row, int grid_column)
{
  return shared_ptr<StaticSprite>(new StaticSprite(image, src_rectangle, position, collision_offset,
                                                   grid_layer, grid_row, grid_column));
}

StaticSprite::StaticSprite(shared_ptr<Image> image, const SDL_Rect &src_rectangle,
                           const SDL_Rect &position, const SDL_Rect &collision_offset,
                           int grid_layer, int grid_row, int grid_column)
    : Sprite(image, src_rectangle, position, collision_offset),
      grid_layer(grid_layer),
      grid_row(grid_row),
      grid_column(grid_column)
{
  debug::analyser.type_constructed(this);
}

StaticSprite::~StaticSprite()
{
  debug::analyser.type_deconstructed(this);
}
