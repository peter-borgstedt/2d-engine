#ifndef TILE_SET_H
#define TILE_SET_H

#include <SDL.h>
#include <memory>
#include <unordered_map>
#include <vector>

#include "Image.h"

struct TMXFrame
{
  int tileid, duration;
};

class Tileset
{
  // gid -> properties (key: string, value string).
  typedef std::unordered_map<int, std::unordered_map<std::string, std::string>>
      properties_unordered_map;
  typedef std::unordered_map<int, std::vector<TMXFrame>> animation_unordered_map;
  typedef std::unordered_map<int, std::vector<SDL_Rect>> object_group_unordered_map;

private:
  const int first_gid, last_gid, gid_width, tile_width, tile_height;
  std::shared_ptr<Image> image;

  // TODO: Remove, populate in TMXParser, do not put these in this class -- these are not being used
  // when the whole content has been loaded.
  const object_group_unordered_map object_group;
  const properties_unordered_map properties;
  const animation_unordered_map animations;

public:
  Tileset(std::shared_ptr<Image>, properties_unordered_map properties,
          animation_unordered_map animations, object_group_unordered_map objectgroup, int first_gid,
          int last_gid, int git_width, int tile_width, int tile_height);
  ~Tileset();

  std::shared_ptr<Image> get_image();

  int get_first_gid() const;
  int get_last_gid() const;
  int get_gid_width() const;
  int get_tile_width() const;
  int get_tile_height() const;

  bool contains_gid(const int &gid);

  // TODO: remove
  bool contains_property(int gid, std::string property);
  const std::string &get_property(int gid, std::string property) const;

  bool contains_object_group(int gid);
  const std::vector<SDL_Rect> &get_objects_group(int gid) const;

  bool contains_animation(int gid);
  std::vector<TMXFrame> const get_animation(int gid) const;
};

#endif
