#include "Sprite.h"
#include "CollisionUtils.h"
#include "Debug.h"

using namespace std;

Sprite::Sprite(shared_ptr<Image> image, const SDL_Rect &src_rectangle, const SDL_Rect &position,
               const SDL_Rect &collision_offset)
    : image(image),
      src_rectangle(src_rectangle),
      position(position),
      collision_offset(collision_offset)
{
  debug::analyser.type_constructed(this);
}

Sprite::~Sprite()
{
  debug::analyser.type_deconstructed(this);
}

void Sprite::paint(SDL_Renderer *renderer, const int &offset_x, const int &offset_y)
{
  SDL_Rect offset_area = position;
  offset_area.x -= offset_x;
  offset_area.y -= offset_y;
  SDL_RenderCopy(renderer, image->get_texture(), &get_source_rectangle(), &offset_area);
}

SDL_Rect &Sprite::get_position()
{
  return position;
}

SDL_Rect &Sprite::get_source_rectangle()
{
  return src_rectangle;
}

SDL_Rect Sprite::get_collision_position()
{
  if (is_collidable()) {
    return {position.x + collision_offset.x, position.y + collision_offset.y, collision_offset.w,
            collision_offset.h};
  }
  return {0, 0, 0, 0};
}

bool Sprite::is_collidable()
{
  return collision_offset.w != 0 && collision_offset.h != 0;
}

const std::shared_ptr<Image> Sprite::get_image() const
{
  return image;
}
