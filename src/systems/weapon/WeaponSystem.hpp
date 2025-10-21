#pragma once
#include "systems/IUpdateSystem.hpp"

class WeaponSystem : public Flag::IUpdateSystem {
public:
  WeaponSystem() = default;
  ~WeaponSystem() = default;
  void update() override;
};
