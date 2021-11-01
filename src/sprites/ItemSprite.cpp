#include "ItemSprite.h"
#include "Debug.h"

using namespace std;
;

shared_ptr<ItemSprite>
ItemSprite::get_instance(shared_ptr<Image> image, const SDL_Rect &src_rectangle,
                         const SDL_Rect &position, const SDL_Rect &collision_offset, int grid_layer,
                         int grid_row, int grid_column, const shared_ptr<FrameAnimation> &animation)
{
  return shared_ptr<ItemSprite>(new ItemSprite(image, src_rectangle, position, collision_offset,
                                               grid_layer, grid_row, grid_column, animation));
}

ItemSprite::ItemSprite(shared_ptr<Image> image, const SDL_Rect &src_rectangle,
                       const SDL_Rect &position, const SDL_Rect &collision_offset, int grid_layer,
                       int grid_row, int grid_column, const shared_ptr<FrameAnimation> &animation)
    : AnimatedSprite(image, src_rectangle, position, collision_offset, grid_layer, grid_row,
                     grid_column, animation)
{
  debug::analyser.type_constructed(this);
}

ItemSprite::~ItemSprite()
{
  debug::analyser.type_deconstructed(this);
}

void ItemSprite::do_logic(shared_ptr<Context> context, shared_ptr<Sprite> trigger)
{
  // Let's apply the rule to all physics sprite. If any of them come into
  // contact with this sprite it will remove it.
  if (shared_ptr<PhysicsSprite> physicsSprite = dynamic_pointer_cast<PhysicsSprite>(trigger)) {
    // Get a centroid of player position, we want to do a collision in the
    // center of the portal area.
    SDL_Rect centroid = physicsSprite->get_position();
    centroid.x = centroid.x + (centroid.w / 2);
    centroid.y = centroid.y + (centroid.h / 2);

    if (!collision_utils::no_collision(centroid, position)) {
      state = kOBSOLETED;
      shared_ptr<Map> map = context->get_map_manager()->current_map();
      map->get_sprite_manager()->remove_static_sprite(grid_layer, grid_row, grid_column);
    }
  }
}
