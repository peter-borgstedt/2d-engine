#include "EventManager.h"
#include "Debug.h"

EventManager::EventManager()
{
  debug::analyser.type_constructed(this);
}

EventManager::~EventManager()
{
  debug::analyser.type_deconstructed(this);
}

void EventManager::map_key(SDL_Keycode key_code, event_function function)
{
  key_mappings.insert({key_code, function});
}

void EventManager::trigger_event(std::shared_ptr<Context> context, const SDL_Event &event)
{
  /*
          for (auto kv : key_mappings)
                  if (event.key.keysym.sym == kv.first)
                          kv.second(context, event);
   */
  if (key_mappings.count(event.key.keysym.sym)) {
    key_mappings[event.key.keysym.sym](context, event);
  }
}
