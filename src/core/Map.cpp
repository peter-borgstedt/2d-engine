#include "Map.h"

#include <algorithm>
#include <array>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "Common.h"
#include "Context.h"
#include "Debug.h"
#include "Utility.h"

using namespace std;

Map::Map(const SDL_Rect &map_dimension, shared_ptr<Window> window, const MapMetrics &map_metrics)
    : map_dimension(map_dimension),
      window(window),
      map_metrics(map_metrics),
      camera(map_metrics),
      retriever(),
      sprite_manager(make_shared<SpriteManager>(map_metrics, map_dimension))
{
  debug::analyser.type_constructed(this);
}

std::shared_ptr<Map> Map::get_instance(const SDL_Rect &map_dimension,
                                       std::shared_ptr<Window> window,
                                       const MapMetrics &map_metrics)
{
  return std::shared_ptr<Map>(new Map(map_dimension, window, map_metrics));
}

Map::~Map()
{
  debug::analyser.type_deconstructed(this);
}

std::shared_ptr<SpriteManager> Map::get_sprite_manager()
{
  return sprite_manager;
}

// For details on rendering, se See SpriteManager::paint(SDL_Renderer*, Camera&, const SDL_Rect&).
void Map::paint(SDL_Renderer *renderer)
{
  sprite_manager->paint(renderer, camera, map_dimension);
}

const SDL_Rect &Map::get_map_dimension()
{
  return map_dimension;
}

const MapMetrics &Map::get_map_metrics() const
{
  return map_metrics;
}

const void Map::update_map_dimension()
{
  map_dimension = window->get_logical_size();
}

void Map::tick(std::shared_ptr<Context> context)
{
  // The quad tree has flaws, at the moment it is very unoptimized -- ignore it for now.
  // TODO: fix bottleneck parts and make it work, it will give use more benefits and speed.
  // shared_ptr<QuadTree> quad_tree = sprite_manager->get_quad_tree();
  // quad_tree->reset(map_metrics.map_dimension);

  vector<shared_ptr<Sprite>> &dynamic_sprites = sprite_manager->get_dynamic_sprites();

  for (auto ds : dynamic_sprites) {
    dynamic_pointer_cast<DynamicSprite>(ds)->tick();
    // TODO: fix the quad tree, ignore now.
    // quad_tree->insert(ds);
  }

  for (auto it = dynamic_sprites.begin(); it != dynamic_sprites.end();) {
    shared_ptr<DynamicSprite> ds = dynamic_pointer_cast<DynamicSprite>(*it);

    // TODO: fix the quad tree, ignore for now.
    // quad_tree->retrieve(retriever, ds);
    sprite_manager->retrieve_sprites(utility::getScope(ds->get_position()), retriever);

    // Replacing the quad tree with a raw insert of all dynamic sprites.
    // This will be fine until be reach to a large point of dynamic sprites.
    retriever.insert(retriever.end(), dynamic_sprites.begin(), dynamic_sprites.end());

    // Determine what to do with previous action.
    ds->determine(context, retriever);

    if (ds->state == Sprite::State::kOBSOLETED) {
      it = dynamic_sprites.erase(it);
      continue;
    } else {
      ++it;
    }
    retriever.clear();
    sprite_manager->add_dirty_sprite(ds);
  }
}
