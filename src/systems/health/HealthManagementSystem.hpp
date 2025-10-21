#pragma once
#ifndef HEALTHMANAGEMENTSYSTEM_HPP
#define HEALTHMANAGEMENTSYSTEM_HPP

#include "systems/IReactiveSystem.hpp"
#include "systems/IRenderSystem.hpp"
#include "systems/IUpdateSystem.hpp"
class HealthManagementSystem : public Flag::IRenderSystem,
                               public Flag::IReactiveSystem,
                               public Flag::IUpdateSystem {
public:
  HealthManagementSystem() = default;
  ~HealthManagementSystem() = default;

  void render() override;
  void on_message(const Flag::IMessage &message) override;
  void update() override;
};
#endif // HEALTHMANAGEMENTSYSTEM_HPP
