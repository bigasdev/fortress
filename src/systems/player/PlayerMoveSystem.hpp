#pragma once

#include "systems/IUpdateSystem.hpp"

class PlayerMoveSystem : public Flag::IUpdateSystem {
public:
  void update() override;
};
