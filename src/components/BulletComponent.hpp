#pragma once
#include "../tools/Common.hpp"
#include "components/IComponent.hpp"

class BulletComponent : public Flag::IComponent {
public:
  ~BulletComponent() = default;

  int id = 0;
  bool rotates = false;
  float speed = 300.0f;
  float rotation_speed = 15.0f;
  double dx, dy;

  float alive_for = 0.0f;
  float life_time = .5f;
};
