#include "Tileset.h"
#include "Debug.h"
#include <sstream>

using namespace std;

Tileset::Tileset(std::shared_ptr<Image> image, properties_unordered_map properties,
                 animation_unordered_map animations, object_group_unordered_map objectgroup,
                 int first_gid, int last_gid, int git_width, int tile_width, int tile_height)
    : image(image),
      properties(properties),
      animations(animations),
      object_group(objectgroup),
      first_gid(first_gid),
      last_gid(last_gid),
      gid_width(git_width),
      tile_width(tile_width),
      tile_height(tile_height)
{
  debug::analyser.type_constructed(this);
}

Tileset::~Tileset()
{
  debug::analyser.type_deconstructed(this);
}

std::shared_ptr<Image> Tileset::get_image()
{
  return image;
}

int Tileset::get_gid_width() const
{
  return gid_width;
}

int Tileset::get_tile_width() const
{
  return tile_width;
}

int Tileset::get_tile_height() const
{
  return tile_height;
}

bool Tileset::contains_gid(const int &gid)
{
  return gid >= first_gid && gid <= last_gid;
}

int Tileset::get_first_gid() const
{
  return first_gid;
}

int Tileset::get_last_gid() const
{
  return last_gid;
}

bool Tileset::contains_property(int gid, string property)
{
  if (properties.count(gid)) {
    return properties.at(gid).count(property) != 0;
  }
  return false;
}

// NOTE: this function is unsafe, validate first.
const std::string &Tileset::get_property(int gid, std::string property) const
{
  if (properties.count(gid) && properties.at(gid).count(property)) {
    return properties.at(gid).at(property);
  }
  throw runtime_error("Tileset: missing property '" + property + "' for gid " + to_string(gid));
}

bool Tileset::contains_object_group(int gid)
{
  return object_group.count(gid) != 0;
}

// NOTE: this function is unsafe, validate first.
const std::vector<SDL_Rect> &Tileset::get_objects_group(int gid) const
{
  if (object_group.count(gid)) {
    return object_group.at(gid);
  }
  throw runtime_error("Tileset: missing group data for gid " + to_string(gid));
}

bool Tileset::contains_animation(int gid)
{
  return animations.count(gid) != 0;
}

// NOTE: this function is unsafe, validate first.
vector<TMXFrame> const Tileset::get_animation(int gid) const
{
  if (animations.count(gid)) {
    return animations.at(gid);
  }
  throw runtime_error("Tileset: animation data for gid " + to_string(gid));
}
