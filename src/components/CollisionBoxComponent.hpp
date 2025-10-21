#pragma once
#include "../tools/Common.hpp"
#include "components/IComponent.hpp"
#include "entity/Types.hpp"

class CollisionBoxComponent : public Flag::IComponent {
public:
  ~CollisionBoxComponent() = default;

  int parent = -1;
  vec2 size = {16, 16};
  vec2 offset = {0, 0};
  ColLayers layer = ColLayers::DEFAULT;
  Col gizmo_color = Col(255, 0, 0, 100);
  bool touching_something = false;
};
