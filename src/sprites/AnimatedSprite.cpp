#include "AnimatedSprite.h"

#include "CollisionUtils.h"
#include "Debug.h"
#include "PhysicsSprite.h"

using namespace std;

shared_ptr<AnimatedSprite>
AnimatedSprite::get_instance(shared_ptr<Image> image, const SDL_Rect &src_rectangle,
                             const SDL_Rect &position, const SDL_Rect &collision_offset,
                             int grid_layer, int grid_row, int grid_column,
                             const std::shared_ptr<FrameAnimation> &animation)
{
  return shared_ptr<AnimatedSprite>(new AnimatedSprite(image, src_rectangle, position,
                                                       collision_offset, grid_layer, grid_row,
                                                       grid_column, animation));
}

AnimatedSprite::AnimatedSprite(shared_ptr<Image> image, const SDL_Rect &src_rectangle,
                               const SDL_Rect &position, const SDL_Rect &collision_offset,
                               int grid_layer, int grid_row, int grid_column,
                               const std::shared_ptr<FrameAnimation> &animation)
    : StaticSprite(image, src_rectangle, position, collision_offset, grid_layer, grid_row,
                   grid_column),
      animation(animation)
{
  debug::analyser.type_constructed(this);
}

AnimatedSprite::~AnimatedSprite()
{
  debug::analyser.type_deconstructed(this);
}

SDL_Rect &AnimatedSprite::get_source_rectangle()
{
  return src_rectangle = animation->get_next_source();
}
