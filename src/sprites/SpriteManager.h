#ifndef SPRITE_MANAGER_H
#define SPRITE_MANAGER_H

#include <SDL.h>
#include <functional>
#include <memory>
#include <set>
#include <vector>

#include "DataContainer.h"

#include "Camera.h"

#include "QuadTree.h"

#include "Tileset.h"

#include "DynamicSprite.h"
#include "PlayerSprite.h"
#include "Sprite.h"

#include "ParticleSprite.h"

class SpriteManager
{
  // Use unsigned 64bit int for key instead of a complex key or a nested map
  // structure.
  typedef std::unordered_map<uint64_t, std::shared_ptr<Sprite>> sprite_matrix;

  template <typename T>
  using sprite_vector = std::vector<std::shared_ptr<T>>; // This is generic beauty!
private:
  const MapMetrics map_metrics;

  // TODO: we want to support several players (and probably put them with the
  // dynamic sprites).
  // TODO: we should adjust so we don't need the PlayerSprite typename --
  // without casting.
  // TODO: limit sprites to StaticSprite and DynamicSprite.
  std::shared_ptr<PlayerSprite> player;

  // Amount of layers in the static sprite grid/matrix.
  int static_sprites_layer_size = 0;

  // A grid/grid containing sprites with a constant position.
  sprite_matrix static_sprites;

  // Collection of dynamic sprites with undetermined position.
  sprite_vector<Sprite> dynamic_sprites;
  // TODO: use the this one when the quad tree is working properly.
  // sprite_vector<DynamicSprite> dynamic_sprites;

  // Quad tree constantly updated with dynamic sprites. Spatial indexed, for
  // fast lookup.
  // TODO: the quad tree does not work that well at the moment, some adjustments
  // need to be done it useful (i.e. really fast).
  std::shared_ptr<QuadTree> quad_tree;

  // Keep track on sprites to be redrawn.
  // sprite_vector<Sprite> dirty_sprites;
  std::vector<std::shared_ptr<Sprite>> dirty_sprites;

  // Images to be rendered in the background.
  std::vector<std::shared_ptr<Image>> background_images;

  // Tileset to sheet positions. This is TMX-specific and should really be in
  // here.
  // TODO: move this else where.
  std::vector<std::shared_ptr<Tileset>> tilesets;

  // Do bit-manipulation on layer, row and column and combine into a single
  // uint64_t (unsigned long long) for a compact lookup, this reduces overhead
  // when using nested maps and so on. Combine 16 bit sections as:
  // [unused]-[i:layer]-[y:row]-[x:column]
  inline uint64_t get_static_key(const uint16_t &layer, const uint16_t &row, const uint16_t &column)
  {
    return layer << 2 * 8 | row << 8 | column;
  }

  sprite_vector<Sprite> sprite_collector;

public:
  SpriteManager(const MapMetrics &metrics, const SDL_Rect &bounds);
  ~SpriteManager();

  void paint(SDL_Renderer *renderer, Camera &camera, const SDL_Rect &clip);

  std::shared_ptr<Sprite> get_static_sprite(const uint16_t &layer, const uint16_t &row,
                                            const uint16_t &column);

  void add_dirty_sprite(std::shared_ptr<Sprite> sprite);

  std::vector<std::shared_ptr<Sprite>> &get_dirty_sprites();

  // Add a static sprite to the grid.
  void add_static_sprite(const uint16_t &layer, const uint16_t &row, const uint16_t &column,
                         std::shared_ptr<Sprite> sprite);

  // Remove a static sprite from the grid.
  void remove_static_sprite(int layer, int row, int column);

  // Add a dynamic sprite to the collection.
  void add_dynamic_sprite(std::shared_ptr<DynamicSprite> sprite);

  // Get the quad tree.
  std::shared_ptr<QuadTree> get_quad_tree();

  // Retrieve and process each sprite into a given function/functor.
  void do_retrieve_sprites(const Camera::Scope &scope,
                           std::function<void(std::shared_ptr<Sprite>)> f);

  // Retrieve sprites into given vector.
  void retrieve_sprites(const Camera::Scope &scope, std::vector<std::shared_ptr<Sprite>> &sprites);

  // Get all dynamic sprites.
  std::vector<std::shared_ptr<Sprite>> &SpriteManager::get_dynamic_sprites();

  // Add a background image.
  void add_background(std::shared_ptr<Image> background_image);

  // Get background image.
  const std::vector<std::shared_ptr<Image>> &get_backgrounds();

  // Set player sprite, one is only supported for now.
  void set_player(std::shared_ptr<PlayerSprite> player);

  std::shared_ptr<PlayerSprite> get_player();

  // Add a tileset, this is TMX-specific and should be moved else where.
  void add_tileset(std::shared_ptr<Tileset> tileset);

  // Add a tileset, this is TMX-specific and should be moved else where.
  std::vector<std::shared_ptr<Tileset>> &get_tilesets();

  // Create a tiled sprite. Move this else where.
  void create_tiled_sprite();

  // Create a tiled sprite. Move this else where.
  void create_player_sprite();
};

#endif
