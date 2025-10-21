#pragma once
#include "systems/IRenderSystem.hpp"
#include "systems/IUpdateSystem.hpp"
#include <vector>

class BulletSystem : public Flag::IUpdateSystem, public Flag::IRenderSystem {
public:
  BulletSystem() = default;
  ~BulletSystem() = default;
  void update() override;
  void render() override;

private:
  std::vector<int> m_curr_spatial_entities;
};
