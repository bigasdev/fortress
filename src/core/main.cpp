/**
  @description starting point of the game, act as a boot to load the Engine and
targetting it to the corect platform
**/
#ifndef MAIN_HPP
#define MAIN_HPP

#include "SDL_timer.h"
#ifdef __EMSCRIPTEN__
#include "emscripten.h"
#endif

#include "../tools/Logger.hpp"
#include "Assert.hpp"
#include "Engine.hpp"
#include "Timer.hpp"
#include <iostream>
#include <memory>

std::unique_ptr<Engine> engine;

void setup() {
  engine = std::make_unique<Engine>();
  engine->init();
}

void loop() {
#ifdef __EMSCRIPTEN__
  // mainloop
#elif __WIN32__
  // dt calculation is always done at the start of the frame
  Timer::update();
  engine->input();

  // fixed delta time loop
  while (Timer::get_accumulator() >= Timer::get_tmod()) {
    Timer::fixed_t();
    engine->fixed_update();
  }
  engine->update();
  engine->post_update();
  engine->draw();
#endif
  engine->post_init();

#ifdef __EMSCRIPTEN__
  emscripten_cancel_main_loop();
#endif
}

int main(int args, char *argv[]) {
  setup();

#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop(loop, 0, 1);
#else
  while (engine->is_running()) {
    loop();
  }
#endif

  engine->quit();
  return 0;
}
#endif
