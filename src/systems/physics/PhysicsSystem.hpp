#pragma once
#ifndef PHYSICSSYSTEM_HPP
#define PHYSICSSYSTEM_HPP

#include "systems/IFixedUpdateSystem.hpp"
#include "systems/IUpdateSystem.hpp"
class PhysicsSystem : public Flag::IUpdateSystem,
                      public Flag::IFixedUpdateSystem {
public:
  PhysicsSystem() = default;
  ~PhysicsSystem() = default;

  void update() override;
  void fixed_update() override;

private:
  float gravity = 9.81f;
};
#endif // PHYSICSSYSTEM_HPP
