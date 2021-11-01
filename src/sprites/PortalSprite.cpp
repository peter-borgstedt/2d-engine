#include "PortalSprite.h"
#include "CollisionUtils.h"
#include "Debug.h"

#include "PlayerSprite.h"

using namespace std;

shared_ptr<PortalSprite>
PortalSprite::get_instance(shared_ptr<Image> image, const SDL_Rect &src_rectangle,
                           const SDL_Rect &position, const SDL_Rect &collision_offset,
                           int grid_layer, int grid_row, int grid_column, const int &map_index,
                           const Direction &direction)
{
  return shared_ptr<PortalSprite>(new PortalSprite(image, src_rectangle, position, collision_offset,
                                                   grid_layer, grid_row, grid_column, map_index,
                                                   direction));
}

PortalSprite::PortalSprite(shared_ptr<Image> image, const SDL_Rect &src_rectangle,
                           const SDL_Rect &position, const SDL_Rect &collision_offset,
                           int grid_layer, int grid_row, int grid_column, const int &map_index,
                           const Direction &direction)
    : StaticSprite(image, src_rectangle, position, collision_offset, grid_layer, grid_row,
                   grid_column),
      map_index(map_index),
      direction(direction)
{
  debug::analyser.type_constructed(this);
}

PortalSprite::~PortalSprite()
{
  debug::analyser.type_deconstructed(this);
}

void PortalSprite::do_logic(shared_ptr<Context> context, shared_ptr<Sprite> trigger)
{
  if (shared_ptr<PlayerSprite> player = dynamic_pointer_cast<PlayerSprite>(trigger)) {
    // Get a centroid of player position, we want to do a collision in the
    // center of the portal area.
    SDL_Rect centroid = player->get_position();
    centroid.x = centroid.x + (centroid.w / 2);
    centroid.y = centroid.y + (centroid.h / 2);

    if (!collision_utils::no_collision(centroid, position)) {
      // TODO: add option to unload map content if there is not returning back
      // to this map.
      context->get_map_manager()->change_map(map_index);

      // Adjust player offset of portal and turn player to portal output
      // position. This is done because we might want to go back later on -- and
      // have a correct direction and position.
      SDL_Rect &player_position = player->get_position();
      player->set_direction(direction);

      if (direction == kLEFT) {
        player_position.x = position.x - player_position.w;
      } else {
        player_position.x = position.x + position.w;
      }
      player->set_movement(false); // Stop.
    }
  }
}
