#pragma once
#include "systems/IPauseSystem.hpp"
#include "systems/IRenderSystem.hpp"
#include "systems/IUpdateSystem.hpp"

class AnimatedSpriteSystem : public Flag::IUpdateSystem,
                             public Flag::IRenderSystem,
                             public Flag::IPauseSystem {
public:
  AnimatedSpriteSystem() = default;
  ~AnimatedSpriteSystem() = default;
  void update() override;
  void render() override;
  void pause() override;
};
