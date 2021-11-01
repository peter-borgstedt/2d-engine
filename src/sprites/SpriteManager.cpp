#include "SpriteManager.h"
#include "Debug.h"
#include <algorithm>

using namespace std;

SpriteManager::SpriteManager(const MapMetrics &map_metrics, const SDL_Rect &bounds)
    : map_metrics(map_metrics),
      static_sprites(),
      dynamic_sprites(),
      dirty_sprites(),
      quad_tree(QuadTree::get_instance(0, bounds)),
      tilesets()
{
  debug::analyser.type_constructed(this);
}

SpriteManager::~SpriteManager()
{
  debug::analyser.type_deconstructed(this);
}

// For debugging purpose, paint quad tree zones.
// TODO: move this to a more proper place, for name use an anonymous namespace
// to keep this in the a private scope.
namespace
{
  void draw_nodes_bounds(SDL_Renderer *renderer, SDL_Rect camera_position, QuadTree *quad_tree)
  {
    const auto &nodes = quad_tree->get_nodes();
    const auto &tl_node = nodes[0];
    const auto &tr_node = nodes[1];
    const auto &bl_node = nodes[2];
    const auto &br_node = nodes[3];

    if (!tl_node) {
      return;
    }

    const SDL_Rect &tlb = tl_node->get_bounds();
    const SDL_Rect &trb = tr_node->get_bounds();
    const SDL_Rect &blb = bl_node->get_bounds();
    const SDL_Rect &brb = br_node->get_bounds();

    int x1 = tlb.x;
    int x2 = tlb.x + tlb.w * 2; // trb.x + trb.w;
    int y = tlb.y + tlb.h;
    x1 -= camera_position.x;
    x2 -= camera_position.x;
    y -= camera_position.y;

    int y1 = tlb.y;
    int y2 = brb.y + brb.h;
    int x = trb.x;
    y1 -= camera_position.y;
    y2 -= camera_position.y;
    x -= camera_position.x;

    SDL_RenderDrawLine(renderer, x1, y, x2, y);
    SDL_RenderDrawLine(renderer, x, y1, x, y2);

    for (auto node : quad_tree->get_nodes())
      draw_nodes_bounds(renderer, camera_position, node);
  }

  void paint_quad_tree_and_returning_objects(SDL_Renderer *renderer, SDL_Rect camera_position,
                                             shared_ptr<QuadTree> quad_tree,
                                             vector<shared_ptr<Sprite>> return_objects)
  {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 128);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // Draw objects retrieved from quad zones -- focused from player position.
    for (auto r : return_objects) {
      SDL_Rect rr = r->get_position();
      rr.x -= camera_position.x;
      rr.y -= camera_position.y;
      SDL_RenderFillRect(renderer, &rr);
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    SDL_SetRenderDrawColor(renderer, 225, 225, 225, 255);

    // Draw zone boundaries.
    draw_nodes_bounds(renderer, camera_position, quad_tree.get());
  }
} // namespace

// Rendering is the big performance thief in SDL -- and is an issue when doing a
// lot of small rendering. SDL has no support for batch rendering, this would be
// the fastest and preferable way to go. The solution to this bottleneck is to
// use Open-GL with SDL and do this by yourself, or even better don't use SDL,
// there are better libraries for this (like SFML). Also Open-GL has better
// support if we want to use a frame buffer, SDL_Texture's have limited size.
// See discussion below for details about all of this (or any other forums on
// the internet this problem):
// https://www.reddit.com/r/gamedev/comments/3y1va8/sdl2_c_tile_map_most_efficient_way_to_render_lots/
// http://gamedev.stackexchange.com/questions/30362/drawing-lots-of-tiles-with-opengl-the-modern-way
//
// TODO: Render in layer order.
void SpriteManager::paint(SDL_Renderer *renderer, Camera &camera, const SDL_Rect &clip)
{
  SDL_Rect camera_position = camera.get_camera_position();
  for (auto image_background : background_images) {
    SDL_RenderCopy(renderer, image_background->get_texture(), NULL,
                   &image_background->get_boundary());
  }

  Camera::Scope scope = camera.focus_camera(player->get_position(), clip);
  retrieve_sprites(scope, dirty_sprites);

  dirty_sprites.push_back(player);
  while (dirty_sprites.size() > 0) {
    dirty_sprites.back()->paint(renderer, camera_position.x, camera_position.y);
    dirty_sprites.pop_back();
  }

  quad_tree->retrieve(sprite_collector, player);
  paint_quad_tree_and_returning_objects(renderer, camera_position, quad_tree, sprite_collector);
  sprite_collector.clear();
}

shared_ptr<Sprite> SpriteManager::get_static_sprite(const uint16_t &layer, const uint16_t &row,
                                                    const uint16_t &column)
{
  uint64_t key = get_static_key(layer, row, column);
  return static_sprites.count(key) ? static_sprites.at(key) : nullptr;
}

void SpriteManager::add_dirty_sprite(shared_ptr<Sprite> sprite)
{
  dirty_sprites.push_back(sprite);
}

vector<shared_ptr<Sprite>> &SpriteManager::get_dirty_sprites()
{
  return dirty_sprites;
}

void SpriteManager::add_static_sprite(const uint16_t &layer, const uint16_t &row,
                                      const uint16_t &column, shared_ptr<Sprite> sprite)
{
  if (layer > static_sprites_layer_size) {
    static_sprites_layer_size = layer;
  }
  static_sprites[get_static_key(layer, row, column)] = sprite;
}

void SpriteManager::remove_static_sprite(int layer, int row, int column)
{
  static_sprites.erase(get_static_key(layer, row, column));
}

void SpriteManager::add_dynamic_sprite(shared_ptr<DynamicSprite> sprite)
{
  dynamic_sprites.push_back(sprite);
}

shared_ptr<QuadTree> SpriteManager::get_quad_tree()
{
  return quad_tree;
}

void SpriteManager::do_retrieve_sprites(const Camera::Scope &scope,
                                        function<void(shared_ptr<Sprite>)> f)
{
  for (int layer = 0; layer <= static_sprites_layer_size; ++layer)
    for (int row = scope.y_min; row <= scope.y_max; ++row)
      for (int column = scope.x_min; column <= scope.x_max; ++column) {
        const uint64_t key = get_static_key(layer, row, column);
        if (static_sprites.count(key)) {
          f(static_sprites.at(key));
        }
      }
}

void SpriteManager::retrieve_sprites(const Camera::Scope &scope,
                                     vector<shared_ptr<Sprite>> &sprites)
{
  do_retrieve_sprites(scope, [&sprites](shared_ptr<Sprite> s) { sprites.push_back(s); });
}

std::vector<std::shared_ptr<Sprite>> &SpriteManager::get_dynamic_sprites()
{
  return dynamic_sprites;
}

void SpriteManager::add_background(shared_ptr<Image> background_image)
{
  background_images.push_back(background_image);
}

const vector<shared_ptr<Image>> &SpriteManager::get_backgrounds()
{
  return background_images;
}

bool cmp(shared_ptr<Sprite> lhs, std::shared_ptr<Sprite> rhs)
{
  return lhs->order > rhs->order;
}

void SpriteManager::set_player(shared_ptr<PlayerSprite> player)
{
  SpriteManager::player = player;
  dynamic_sprites.push_back(player);
  sort(dynamic_sprites.begin(), dynamic_sprites.end(), cmp);
}

shared_ptr<PlayerSprite> SpriteManager::get_player()
{
  return player;
}

void SpriteManager::add_tileset(shared_ptr<Tileset> tileset)
{
  tilesets.push_back(tileset);
}

std::vector<std::shared_ptr<Tileset>> &SpriteManager::get_tilesets()
{
  return tilesets;
}

void SpriteManager::create_tiled_sprite()
{
}

void SpriteManager::create_player_sprite()
{
}
