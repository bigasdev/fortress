#pragma once

#include "systems/IRenderSystem.hpp"
#include "systems/IUpdateSystem.hpp"
#include "tools/Common.hpp"

class PlayerAttackSystem : public Flag::IUpdateSystem,
                           public Flag::IRenderSystem {
public:
  void update() override;
  void render() override;

private:
  vec2 m_last_mouse_pos;
};
