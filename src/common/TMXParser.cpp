#include "TMXParser.h"

#include <exception>
#include <functional>
#include <sstream>

#include "Common.h"
#include "Debug.h"

#include "XMLDoc.h"
#include "XMLNode.h"

#include "Map.h"
#include "ResourceManager.h"

#include "ItemSprite.h"
#include "MovingSprite.h"
#include "PlayerSprite.h"
#include "PortalSprite.h"
#include "StaticSprite.h"

using namespace std;
using namespace XML;

namespace
{
  // Ref: https://google.github.io/styleguide/cppguide.html
  // Using Google's style convention for naming enumerations.
  enum Type
  {
    kTILE,
    kPLAYER,
    kMOVING,
    kMOVABLE,
    kPORTAL,
    kPHYSICS,
    kITEM
  };

  std::unordered_map<std::string, Type> SPRITE_TYPES = {
      {"TILE", kTILE},     {"PLAYER", kPLAYER},   {"MOVING", kMOVING}, {"MOVABLE", kMOVABLE},
      {"PORTAL", kPORTAL}, {"PHYSICS", kPHYSICS}, {"ITEM", kITEM}};

  void load_images(XMLDoc *doc, shared_ptr<Context> context, shared_ptr<Map> map);
  void load_sprites(XMLDoc *doc, std::vector<shared_ptr<Tileset>> &tilesets,
                    std::shared_ptr<Map> map);
  void load_tilesets(XMLDoc *doc, std::vector<shared_ptr<Tileset>> &tilsets,
                     shared_ptr<Context> context, shared_ptr<Map> map);

  SDL_Rect get_dst(const int &x, const int &y, std::shared_ptr<Tileset> tileset,
                   MapMetrics map_metrics)
  {
    int dst_x = x * map_metrics.map_tile_width;
    int dst_y = y * map_metrics.map_tile_height;
    return {dst_x, dst_y, tileset->get_tile_width(), tileset->get_tile_height()};
  }

  SDL_Rect get_src(const int &logical_gid, std::shared_ptr<Tileset> tileset)
  {
    int src_x = logical_gid % tileset->get_gid_width() * tileset->get_tile_width();
    int src_y = logical_gid / tileset->get_gid_width() * tileset->get_tile_height();
    return {src_x, src_y, tileset->get_tile_width(), tileset->get_tile_height()};
  }

  shared_ptr<FrameAnimation> get_animation_frames(shared_ptr<Tileset> tileset, int logical_gid)
  {
    vector<FrameAnimation::Frame> animation_frames;
    if (tileset->contains_animation(logical_gid)) {
      for (const TMXFrame &f : tileset->get_animation(logical_gid)) {
        FrameAnimation::Frame frame;
        frame.rectangle = get_src(f.tileid, tileset);
        frame.duration = f.duration;
        animation_frames.push_back(frame);
      }
    }
    return make_shared<FrameAnimation>(animation_frames);
  }

} // namespace

void TMX::parse(shared_ptr<Context> context, std::string file_path)
{
  XMLDoc *doc = XML::parse_file(file_path);

  tuple<int, int, int, int> map_data;

  XMLNode *map_node = doc->get("?xml\\map\\").at(0); // There will only exist one map element.
  int map_width = map_node->get_int_attribute("width");
  int map_height = map_node->get_int_attribute("height");
  int map_tile_width = map_node->get_int_attribute("tilewidth");
  int map_tile_height = map_node->get_int_attribute("tilewidth");

  MapMetrics map_metrics(map_width, map_height, map_tile_width, map_tile_height);

  // Retrieve object group data.
  vector<XMLNode *> object_data = doc->get("?xml\\map\\objectgroup\\");

  shared_ptr<Window> window = context->get_window();

  SDL_Rect bounds = window->get_logical_size();
  auto map = Map::get_instance(bounds, window, map_metrics);

  // Retrieve tileset data.
  vector<shared_ptr<Tileset>> tilesets;
  load_tilesets(doc, tilesets, context, map);

  // Retrieve image layer
  load_images(doc, context, map);

  // Retrieve layer data.
  load_sprites(doc, tilesets, map);

  delete doc; // Remove document and all underlaying nodes, e.g. everything
              // loaded from the XML.

  context->get_map_manager()->add_map(map);
}

// TODO: TMX-specific, should be moved else where.
shared_ptr<ParticleSprite> TMX::create_particle_sprite(int gid, int x, int y,
                                                       const Direction &direction,
                                                       vector<shared_ptr<Tileset>> &tilesets)
{
  for (auto tileset : tilesets)
    if (tileset->contains_gid(gid)) {
      int logical_gid = gid - tileset->get_first_gid(); // Convert gid to fit texture.
      SDL_Rect src_area = get_src(logical_gid, tileset);
      SDL_Rect dst_area = {x, y, src_area.w, src_area.h};
      return ParticleSprite::get_instance(tileset->get_image(), src_area, dst_area, direction,
                                          get_animation_frames(tileset, logical_gid));
    }
  return nullptr;
}

namespace
{
  void fetch_tile_data(XMLNode *tileset_data, const string &id, function<void(int, XMLNode *)> f)
  {
    for (auto t : tileset_data->get_children("tile"))
      if (XMLNode *parent_node = t->get_first_accounted_child(id)) { // Only one occurrence.
        for (XMLNode *c_node : parent_node->get_all_children())
          f(t->get_int_attribute("id"), c_node);
      }
  }

  unordered_map<int, unordered_map<string, string>> get_properties(XMLNode *tileset_data)
  {
    unordered_map<int, unordered_map<string, string>> properties;
    fetch_tile_data(tileset_data, "properties", [&properties](int gid, XMLNode *c_node) {
      properties[gid][c_node->get_attribute("name")] = c_node->get_attribute("value");
    });
    return properties;
  }

  unordered_map<int, vector<SDL_Rect>> get_object_group(XMLNode *tileset_data)
  {
    unordered_map<int, vector<SDL_Rect>> object_group;
    fetch_tile_data(tileset_data, "objectgroup", [&object_group](int gid, XMLNode *child_node) {
      SDL_Rect rect = {child_node->get_int_attribute("x"), child_node->get_int_attribute("y"),
                       child_node->get_int_attribute("width"),
                       child_node->get_int_attribute("height")};
      object_group[gid].push_back(rect);
    });
    return object_group;
  }

  unordered_map<int, vector<TMXFrame>> get_animations(XMLNode *tileset_data)
  {
    unordered_map<int, vector<TMXFrame>> frames;
    fetch_tile_data(tileset_data, "animation", [&frames](int gid, XMLNode *child_node) {
      frames[gid].push_back(
          {child_node->get_int_attribute("tileid"), child_node->get_int_attribute("duration")});
    });
    return frames;
  }

  shared_ptr<Tileset> get_tileset(vector<shared_ptr<Tileset>> &tilesets, int gid)
  {
    // Retrieve the Tileset that contain this gid.
    for (auto tileset : tilesets)
      if (tileset->contains_gid(gid)) {
        return tileset;
      }
    // This happens if the gid is not contained in any tileset. This will occur
    // you add a tileset using Tiled editor, add new tileset and tiles on a
    // layer then removing the tileset, which will leave old references (this is
    // not cleaned up by the application). This can be noticed when examining
    // the TMX/XML file).
    throw logic_error("Missing tileset for gid: " + to_string(gid));
  }

  Direction get_direction(string direction_str)
  {
    return to_upper(direction_str) == "RIGHT" ? kRIGHT : kLEFT;
  }

  shared_ptr<PlayerSprite> create_player(shared_ptr<Tileset> tileset, int logical_gid, SDL_Rect dst,
                                         SDL_Rect src, SDL_Rect col, int map_tile_height)
  {
    // Adjusting offset when player tile size is larger than map tile size.
    dst.y -= tileset->get_tile_height() - map_tile_height;

    unordered_map<Direction, vector<SDL_Rect>> animation;
    auto anim_f = [&animation, &tileset](vector<string> strings, Direction direction) {
      for (string &s : strings) {
        int anim_gid = stoi(s.c_str());
        int dst_x = anim_gid % tileset->get_gid_width() * tileset->get_tile_width();
        int dst_y = anim_gid / tileset->get_gid_width() * tileset->get_tile_height();
        SDL_Rect dst = {dst_x, dst_y, tileset->get_tile_width(), tileset->get_tile_height()};

        // May not work with larger tiles, use map tile size instead and align
        // with offset.
        animation[direction].push_back(dst);
      }
    };
    // We should do this better. TMX-objects have some nice data -- all sprites
    // should first be parsed TMX-objects. From this we could have some more
    // smart retrieval of all needed data to actually create the sprites.
    Direction direction = get_direction(tileset->get_property(logical_gid, "direction"));

    string anim_right_str = tileset->get_property(logical_gid, "animate_right");
    string anim_left_str = tileset->get_property(logical_gid, "animate_left");

    anim_f(split(anim_right_str, ','), kRIGHT);
    anim_f(split(anim_left_str, ','), kLEFT);

    return PlayerSprite::get_instance(tileset->get_image(), dst, col, animation, direction, 4);
  }

  void load_tilesets(XMLDoc *doc, std::vector<shared_ptr<Tileset>> &tilesets,
                     shared_ptr<Context> context, shared_ptr<Map> map)
  {
    vector<XMLNode *> tileset_nodes = doc->get("?xml\\map\\tileset\\");
    for (XMLNode *tileset_node : tileset_nodes) {
      XMLNode *image_node = tileset_node->get_first_accounted_child("image");

      string path = image_node->get_attribute("source");
      shared_ptr<Image> image = context->get_resource_manager()->get_or_load_image(path);

      int first_gid = tileset_node->get_int_attribute("firstgid");
      int last_gid = first_gid + tileset_node->get_int_attribute("tilecount") - 1;
      int git_width =
          image_node->get_int_attribute("width") / tileset_node->get_int_attribute("tilewidth");
      int tile_width = tileset_node->get_int_attribute("tilewidth");
      int tile_height = tileset_node->get_int_attribute("tileheight");

      auto objectgroup = get_object_group(tileset_node);
      auto properties = get_properties(tileset_node);
      auto animations = get_animations(tileset_node);
      auto tileset = make_shared<Tileset>(image, properties, animations, objectgroup, first_gid,
                                          last_gid, git_width, tile_width, tile_height);
      map->get_sprite_manager()->add_tileset(tileset);
      tilesets.push_back(tileset);
    }
  }

  void load_images(XMLDoc *doc, shared_ptr<Context> context, shared_ptr<Map> map)
  {
    shared_ptr<ResourceManager> resource_manager = context->get_resource_manager();
    shared_ptr<SpriteManager> sprite_manager = map->get_sprite_manager();

    vector<XMLNode *> imagelayer_nodes = doc->get("?xml\\map\\imagelayer\\");
    for (int layer = 0; layer < imagelayer_nodes.size(); layer++) {
      XMLNode *image_layer = imagelayer_nodes.at(0);
      XMLNode *image_node = image_layer->get_first_accounted_child("image");
      string image_path = image_node->get_attribute("source");
      shared_ptr<Image> image = resource_manager->get_or_load_image(image_path);
      sprite_manager->add_background(image);
    }
  }

  void load_sprites(XMLDoc *doc, vector<shared_ptr<Tileset>> &tilesets, shared_ptr<Map> map)
  {
    vector<XMLNode *> layer_nodes = doc->get("?xml\\map\\layer\\");

    MapMetrics map_metrics = map->get_map_metrics();
    const int &map_grid_w = map_metrics.map_grid_width;
    const int &map_grid_h = map_metrics.map_grid_height;
    const int &map_tile_w = map_metrics.map_tile_width;
    const int &map_tile_h = map_metrics.map_tile_height;

    shared_ptr<SpriteManager> sprite_manager = map->get_sprite_manager();
    shared_ptr<PlayerSprite> player; // For now we only support one player.
    for (int layer = 0; layer < layer_nodes.size(); ++layer) {
      // Get layer data.
      XMLNode *layer_data = layer_nodes.at(layer)->get_first_accounted_child("data");

      // Transform data into a vector.
      vector<int> value_ids;
      for (string &s : split(layer_data->get_value(), ','))
        value_ids.push_back(stoi(s));

      // Iterate through layer data, which actually represents a matrix.
      // i: index, y: row, x: column.
      for (int i = 0, x = 0, y = 0; y < map_grid_h; ++i, ++x, x < map_grid_w ? y : (++y, x = 0)) {
        // Use parentheses before decrement to modify the copy not the
        // reference.
        int gid = value_ids[i];
        if (gid > 0) {
          shared_ptr<Tileset> tileset = get_tileset(tilesets, gid);

          // Get a logical gid that fit within the boundary of the texture.
          int logical_gid = gid - tileset->get_first_gid();

          Type type = kTILE;
          if (tileset->contains_property(logical_gid, "type")) {
            type = SPRITE_TYPES[tileset->get_property(logical_gid, "type")];
          }

          // TODO: parse TMX-objects into sprites.
          SDL_Rect collision_area = {0, 0, 0, 0};
          if (tileset->contains_object_group(logical_gid)) {
            collision_area = tileset->get_objects_group(logical_gid).at(0);
          }

          SDL_Rect src = get_src(logical_gid, tileset);
          SDL_Rect dst = get_dst(x, y, tileset, map->get_map_metrics());

          // Finally, lets create some sprites.
          switch (type) {
            case kPLAYER:
              {
                if (!player) { // Only support one player for now.
                  player =
                      create_player(tileset, logical_gid, dst, src, collision_area, map_tile_h);
                }
                break;
              }
            case kTILE:
              {
                auto sprite = StaticSprite::get_instance(tileset->get_image(), src, dst,
                                                         collision_area, layer, y, x);
                sprite_manager->add_static_sprite(layer, y, x, sprite);
                break;
              }
            case kPORTAL:
              {
                // A portal always have a map index to the map to be
                // "transported" into.
                int map_index = stoi(tileset->get_property(logical_gid, "map_index"));
                // A portal always have a out put direction.
                string direction_str = tileset->get_property(logical_gid, "direction");
                Direction direction = get_direction(direction_str);

                auto sprite =
                    PortalSprite::get_instance(tileset->get_image(), src, dst, collision_area,
                                               layer, y, x, map_index, direction);
                sprite_manager->add_static_sprite(layer, y, x, sprite);
                break;
              }
            case kPHYSICS:
              {
                auto sprite =
                    PhysicsSprite::get_instance(tileset->get_image(), src, dst, collision_area);
                sprite_manager->add_dynamic_sprite(sprite);
                break;
              }
            case kITEM:
              {
                auto sprite =
                    ItemSprite::get_instance(tileset->get_image(), src, dst, collision_area, layer,
                                             y, x, get_animation_frames(tileset, logical_gid));
                sprite_manager->add_static_sprite(layer, y, x, sprite);
                break;
              }
            case kMOVING:
              {
                auto sprite = MovingSprite::get_instance(tileset->get_image(), src, dst,
                                                         collision_area, 2, 128);
                sprite_manager->add_dynamic_sprite(sprite);
                break;
              }
            case kMOVABLE:
              break;
          }
        }
      }
    }
    sprite_manager->set_player(player); // Finally add player last.
  }
} // namespace
