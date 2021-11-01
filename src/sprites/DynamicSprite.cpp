#include "DynamicSprite.h"
#include "Debug.h"

using namespace std;

DynamicSprite::DynamicSprite(std::shared_ptr<Image> image, const SDL_Rect &src_rectangle,
                             const SDL_Rect &position, const SDL_Rect &collision_offset)
    : Sprite(image, src_rectangle, position, collision_offset)
{
  debug::analyser.type_constructed(this);
}

DynamicSprite::~DynamicSprite()
{
  debug::analyser.type_deconstructed(this);
}
