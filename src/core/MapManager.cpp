#include "MapManager.h"
#include "Debug.h"

using namespace std;

MapManager::MapManager()
{
  debug::analyser.type_constructed(this);
}

MapManager::~MapManager()
{
  debug::analyser.type_deconstructed(this);
}

void MapManager::add_map(const shared_ptr<Map> map)
{
  map_levels.push_back(map);
}

void MapManager::change_map(const int &index)
{
  map_index = index;
}

std::shared_ptr<Map> MapManager::current_map()
{
  return map_levels.empty() ? nullptr : map_levels[map_index];
}
