#pragma once
#include "systems/IPauseSystem.hpp"
#include "systems/IRenderSystem.hpp"
#include "systems/IStartSystem.hpp"

class SpriteSystem : public Flag::IStartSystem,
                     public Flag::IRenderSystem,
                     public Flag::IPauseSystem {
public:
  SpriteSystem() = default;
  ~SpriteSystem() = default;
  void start() override;
  void render() override;
  void pause() override;
};
