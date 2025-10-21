#pragma once
#ifndef SPRITE_HPP
#define SPRITE_HPP

#include "../tools/Common.hpp"
#include <string>
#include <vector>

struct Sprite {
  // positions

  // basically an offset,this won't take the collisions into account
  std::string sheet = "";
  float spr_x = 0.0f, spr_y = 0.0f;

  // scale

  // collision stuff
  float col_wid = 16, col_hei = 16;
  float col_x = 0.0f, col_y = 0.0f;

  // used to determine the position of the sprite in the Atlas
  float dst_x = 0, dst_y = 0;
  float wid = 16, hei = 16;
  float scale_x = 1.0f, scale_y = 1.0f;
  float squash_x = 1.0f, squash_y = 1.0f;

  // rotation
  float angle = 0;

  // visibility
  int z_order = 0;
  int dir = 1;
  int alpha = 255;
  bool visible = true;
  int pivot_x = 0;
  int pivot_y = 0;

  // shaders
  bool has_shader = false;
  int shader_id = 0;
  int shader_block_id = 0;
  PCol blink_color = WHITE;
  PCol outline_color = WHITE;
};

class SpriteAnimator {
public:
  SpriteAnimator(Sprite *spr);
  ~SpriteAnimator();

  void update(double dt);
  void register_anim();

private:
  Sprite *m_spr;
  float m_timer = 0;
  bool m_playing = false;
};

#endif
