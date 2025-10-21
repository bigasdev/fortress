#include "Camera.hpp"

#include "../core/Timer.hpp"
#include "../tools/Logger.hpp"
#include "../tools/Math.hpp"
#include "SDL_gpu.h"
#include <cmath>
#include <string>

#ifndef GAME_SCALE
#define GAME_SCALE 1
#endif

Camera::Camera(vec2 *size) : m_size(size) {
  m_camera = new GPU_Camera{0, 0, 0, 0, 1, 1, 1, 1};
  m_pos = vec2(0, 0);
  m_tracked_pos = nullptr;

  Logger::log("Game:" + std::to_string(GAME_SCALE));

  m_game_scale = GAME_SCALE;
  tracking_speed = 20.f;

  Logger::log(std::to_string(m_game_scale));
}

GPU_Camera **Camera::get_gpu_cam() { return &m_camera; }

Camera::~Camera() { delete m_camera; }

vec2 Camera::get_pos() { return vec2(m_camera->x, m_camera->y); }

void Camera::move() {
  if (m_tracked_pos != nullptr) {
    last_pos = m_pos;
    auto dead_zone = 8;
    auto tx = m_tracked_pos->x;
    auto ty = m_tracked_pos->y;

    auto dist = Math::dist_vec(m_pos, vec2(tx, ty));
    if (dist >= dead_zone) {
      auto a = std::atan2(ty - m_pos.y, tx - m_pos.x);

      dx +=
          std::cos(a) * (dist - dead_zone) * tracking_speed * Timer::get_tmod();
      dy +=
          std::sin(a) * (dist - dead_zone) * tracking_speed * Timer::get_tmod();
    }

    m_pos.x += dx * Timer::get_dt();
    dx *= Math::pow(base_frict, Timer::get_tmod());
    m_pos.y += dy * Timer::get_dt();
    dy *= Math::pow(base_frict, Timer::get_tmod());

    m_camera->x = (m_pos.x * m_camera->zoom_x - m_size->x * 0.5f);
    m_camera->y = (m_pos.y * m_camera->zoom_y - m_size->y * 0.5f);
  }
}

void Camera::update() {
  if (m_tracked_pos != nullptr) {
    if (Math::fabs(m_pos.x - last_pos.x) >= 1.f ||
        Math::fabs(m_pos.y - last_pos.y) >= 1.f) {
      dirty = true;
    }
  }
}

void Camera::post_update() {
  if (dirty)
    dirty = false;
}

bool Camera::is_on_screen(vec2 pos, float padding) {
  bool left = pos.x + padding < m_pos.x + (m_size->x);
  bool right = m_pos.x - (m_size->x) < pos.x + padding;
  bool top = pos.y + padding < m_pos.y + (m_size->y);
  bool bottom = m_pos.y - (m_size->y) < pos.y + padding;

  return left and right and top and bottom;
}

void Camera::track_pos(vec2 *pos) { m_tracked_pos = pos; }

void Camera::set_zoom(float zoom) {
  m_camera->zoom_x = zoom;
  m_camera->zoom_y = zoom;
}
