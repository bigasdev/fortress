#pragma once
#include "SDL_keycode.h"
#ifndef INPUTMANAGER_HPP
#define INPUTMANAGER_HPP

#include "../tools/Common.hpp"
#include "SDL.h"
#include <map>

enum JoyInput {
  A = 0,
  B = 1,
  X = 2,
  Y = 3,
  L = 4,
  R = 5,
  BACK = 6,
  START = 7,
  JOY_X_CLICK = 8,
  JOY_Y_CLICK = 9,
  LEFT_TRIGGER = 10,
  RIGHT_TRIGGER = 11
};

class InputManager {
public:
  InputManager();
  ~InputManager();

  void bind_keyboard(SDL_KeyCode key, bool *value);
  void bind_joy(JoyInput key, bool *value);
  void bind_mouse(bool *left, bool *right, bool *wheel);
  vec2 get_raw_axis() { return raw_axis; }
  vec2 get_right_axis() { return right_axis; }
  void tick_update();
  void update(SDL_Event event);

  bool get_key_press(SDL_Keycode key, SDL_Keycode mod = SDLK_UNKNOWN);

private:
  std::map<SDL_Keycode, bool *> m_key_map;
  std::map<JoyInput, bool *> m_joy_map;
  vec2 raw_axis;
  vec2 right_axis;
  vec2 axis;

  bool *left_click = nullptr;
  bool *right_click = nullptr;
  bool *wheel_click = nullptr;

  double tick = 0.0;
  double tick_timer = 0.6f;

  bool has_tick = false;
};

#endif
