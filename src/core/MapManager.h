#ifndef S2D_MAP_MANAGER_H
#define S2D_MAP_MANAGER_H

#include <memory>
#include <vector>

class Map;
//#include "Map.h"

class MapManager
{
private:
  int map_index = 0;
  std::vector<std::shared_ptr<Map>> map_levels;

public:
  MapManager();
  ~MapManager();

  void add_map(const std::shared_ptr<Map> map);
  void change_map(const int &index);
  std::shared_ptr<Map> current_map();
};

#endif
