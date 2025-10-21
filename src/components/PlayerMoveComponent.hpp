#pragma once
#include "../tools/Common.hpp"
#include "components/IComponent.hpp"

class PlayerMoveComponent : public Flag::IComponent {
public:
  ~PlayerMoveComponent() = default;

  float speed = 0;
};
