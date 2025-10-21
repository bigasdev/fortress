#pragma once
#include "systems/IReactiveSystem.hpp"
#include "systems/IRenderSystem.hpp"
#include "systems/ISystem.hpp"
#include "systems/IUpdateSystem.hpp"

class CollisionSystem : public Flag::IRenderSystem,
                        public Flag::IUpdateSystem,
                        public Flag::IReactiveSystem {
public:
  CollisionSystem() = default;
  ~CollisionSystem() = default;

  void update() override;
  void render() override;
  void on_message(const Flag::IMessage &message) override;
};
