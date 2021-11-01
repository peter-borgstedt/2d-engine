#include "Engine.h"

#include "Debug.h"
#include "TMXParser.h"

#include "FPSCount.h"
#include "Timer.h"

using namespace std;

Engine::Engine(shared_ptr<Context> context) : context(context)
{
  debug::analyser.type_constructed(this);
}

Engine::~Engine()
{
  debug::analyser.type_deconstructed(this);
}

shared_ptr<Engine> Engine::get_instance(shared_ptr<Context> context)
{
  return shared_ptr<Engine>(new Engine(context));
}

int Engine::run()
{

  /*
   * The FPSCount::start has a optio to set maximum_fps (this i default -1), this lock the FPS to
   * given rate. If set it will be used when calculating delay to keep given frame rate set.
   * VSYNC is now activated (and 60hz give usually around 60fps, if no large bottlenecks). The
   * FPS will now depend on screen refresh rate -- there for might want to set a lock to 60fps, if
   * say the computer screen is set on 100hz.
   * TODO: set FPS-lock when instantiating the Engine or the Window.
   * Test this by just add a number into the start, e.g. counter.start(40); for 40fps.
   */
  FPSCount counter;
  // Set max fps if it should be locked. Now its locked to VSYNC, both can also be combined.
  counter.start(/* 65 */);

  bool program_is_running = true;
  while (program_is_running) {
    /* TODO: use this for key mappings instead, its give a more reliable key state, some keys
       do not act good combined when using the SDL_Event.
       const Uint8 *state = SDL_GetKeyboardState(NULL);
        if (state[SDL_SCANCODE_UP]) {
            debug::log("up key");
        }
     */
    shared_ptr<Map> map = context->get_map_manager()->current_map();
    if (!map) {
      break;
    }

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        program_is_running = false;
        break;
      }

      if (event.type & (SDL_KEYUP | SDL_KEYDOWN)) {
        context->get_event_manager()->trigger_event(context, event);
      }
    }
    map->tick(context);

    context->set_int_data("FPS", counter.get_avarage_fps());
    context->get_engine_core()->paint(context);
    counter.tick();
  }
  return 0;
}
