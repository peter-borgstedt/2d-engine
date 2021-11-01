#ifndef S2D_EVENT_MANAGER_H
#define S2D_EVENT_MANAGER_H

#include <SDL.h>

#include <functional>
#include <memory>
#include <unordered_map>

class Context; // Forward declaration

class EventManager
{
  typedef std::function<void(std::shared_ptr<Context>, const SDL_Event &)> event_function;

  template <typename T> using event_map = std::unordered_map<T, event_function>;

private:
  event_map<SDL_EventType> event_mapping;
  event_map<SDL_Keycode> key_mappings;

public:
  EventManager();
  ~EventManager();
  void map_key(SDL_Keycode key_code, event_function);
  void trigger_event(std::shared_ptr<Context> context, const SDL_Event &event);
};

#endif
