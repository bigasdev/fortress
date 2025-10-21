#pragma once
#include "../tools/Common.hpp"
#include "components/IComponent.hpp"

class TransformComponent : public Flag::IComponent {
public:
  ~TransformComponent() = default;

  vec2 pos = {0, 0};
  vec2 scale = {1, 1};
  float rotation = 0.0f;
  bool flip_x = false;
  bool flip_y = false;
  float pivot_x = 0;
  float pivot_y = 0;
};
