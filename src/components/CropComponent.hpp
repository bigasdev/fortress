#pragma once
#include "../tools/Common.hpp"
#include "components/IComponent.hpp"
#include <string>

enum CropType { FRUIT, PLANT, HERB };

class CropComponent : public Flag::IComponent {
public:
  ~CropComponent() = default;

  std::string seed_planted = "";
  int growth_stage = 0;
  int growth_stages = 3;
  float time_planted = 0;
  float time_to_grow = 10.0f;
  float time_elapsed = 0;
  bool is_harvestable = false;
  CropType type = PLANT;
};
