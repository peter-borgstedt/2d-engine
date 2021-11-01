#ifndef S2D_CONTEXT_H
#define S2D_CONTEXT_H

#include <memory>

#include "EngineCore.h"
#include "EventManager.h"
#include "MapManager.h"
#include "ResourceManager.h"
#include "Window.h"

class Context
{
public:
  template <typename T> using data_map = std::unordered_map<std::string, T>;

  ~Context();

  Context(const Context &) = delete;
  const Context &operator=(const Context &) = delete;

  static std::shared_ptr<Context> get_instance(std::shared_ptr<Window> window);

  std::shared_ptr<Window> get_window() const;
  std::shared_ptr<EngineCore> get_engine_core() const;
  std::shared_ptr<EventManager> get_event_manager() const;
  std::shared_ptr<MapManager> get_map_manager() const;
  std::shared_ptr<ResourceManager> get_resource_manager() const;

  void set_int_data(const std::string &key, const int &value);
  int get_int_data(const std::string &key, const int &default_value) const;
  void set_string_data(const std::string &key, const std::string &value);
  std::string get_string_data(const std::string &key, const std::string &default_value) const;

protected:
  Context(std::shared_ptr<Window> window);

private:
  // We intentionally have to data structures separating their type. This is because we do
  // not want to cast strings to int all the time.
  data_map<std::string> string_data;
  data_map<int> int_data;

  std::shared_ptr<Window> window;
  std::shared_ptr<EngineCore> engine_core;
  std::shared_ptr<EventManager> event_manager;
  std::shared_ptr<MapManager> map_manager;
  std::shared_ptr<ResourceManager> resource_manager;
};

#endif
