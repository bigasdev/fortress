#pragma once
#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "../tools/Common.hpp"

class GPU_Camera;

class Camera {
public:
  Camera(vec2 *size);
  ~Camera();

  void move();
  void update();
  void post_update();

  GPU_Camera **get_gpu_cam();
  int get_game_scale() { return m_game_scale; }
  vec2 get_size() { return *m_size; }
  vec2 get_pos();

  bool is_on_screen(vec2 pos, float padding = 0);
  bool is_dirty() { return dirty; }
  void track_pos(vec2 *pos);
  void set_zoom(float zoom);

private:
  GPU_Camera *m_camera;
  vec2 last_pos;
  vec2 m_pos;
  vec2 m_ref_pos;
  vec2 *m_size;
  vec2 *m_tracked_pos;

  int m_game_scale;

  // tracking stuff
  bool clamp_bounds = false;
  bool dirty = true;
  float tracking_speed = 10.f;
  float base_frict = .89f;
  float dx;
  float dy;
  vec2 raw_focus;
  vec2 clamped_focus;
};

#endif
