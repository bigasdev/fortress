#pragma once

#include "systems/IPopupSystem.hpp"
#include "systems/IRenderSystem.hpp"
#include "systems/IUpdateSystem.hpp"

class FarmingPopup : public Flag::IPopupSystem,
                     public Flag::IRenderSystem,
                     public Flag::IUpdateSystem {
public:
  void render() override;
  void update() override;
  void popup() override;
};
