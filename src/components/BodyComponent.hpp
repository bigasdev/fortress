#pragma once
#include "../tools/Common.hpp"
#include "components/IComponent.hpp"

class BodyComponent : public Flag::IComponent {
public:
  ~BodyComponent() = default;

  float dx, dy;
  float weight;
};
