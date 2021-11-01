#include "Context.h"
#include "Debug.h"

using namespace std;

Context::Context(shared_ptr<Window> window)
    : window(window),
      event_manager(make_shared<EventManager>()),
      map_manager(make_shared<MapManager>()),
      resource_manager(make_shared<ResourceManager>()),
      engine_core(EngineCore::get_instance())
{
  debug::analyser.type_constructed(this);
}

Context::~Context()
{
  debug::analyser.type_deconstructed(this);
}

shared_ptr<Context> Context::get_instance(shared_ptr<Window> window)
{
  return shared_ptr<Context>(new Context(window));
}

shared_ptr<EngineCore> Context::get_engine_core() const
{
  return engine_core;
}

shared_ptr<Window> Context::get_window() const
{
  return window;
}

shared_ptr<EventManager> Context::get_event_manager() const
{
  return event_manager;
}

shared_ptr<MapManager> Context::get_map_manager() const
{
  return map_manager;
}

shared_ptr<ResourceManager> Context::get_resource_manager() const
{
  return resource_manager;
}

void Context::set_int_data(const std::string &key, const int &value)
{
  int_data[key] = value;
}

int Context::get_int_data(const std::string &key, const int &default_value) const
{
  return int_data.count(key) ? int_data.at(key) : default_value;
}

void Context::set_string_data(const std::string &key, const std::string &value)
{
  string_data[key] = value;
}

std::string Context::get_string_data(const std::string &key, const std::string &default_value) const
{
  return string_data.count(key) ? string_data.at(key) : default_value;
}
