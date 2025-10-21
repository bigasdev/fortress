#pragma once

/*
   Here is where all the game logic will be, the goal is to never touch the
   Engine code, just use this one to manage the game.
 */

#ifndef GAME_HPP
#define GAME_HPP

class Camera;
class Cooldown;

class Game {
public:
  Game();
  ~Game();

  void init();
  void fixed_update(double tmod);
  void update(double dt);
  void post_update(double dt);
  void draw_root();
  void draw_ent();
  void draw_ui();
  void clean();

private:
  Camera *m_camera;
  Cooldown *m_cooldown;
};

#endif
