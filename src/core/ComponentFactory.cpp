#include "ComponentFactory.hpp"
#include "../components/PlayerMoveComponent.hpp"
#include "../components/SpriteComponent.hpp"
#include "../components/TransformComponent.hpp"
#include "../components/WeaponComponent.hpp"

ComponentFactory::ComponentFactory() {
  register_component("SpriteComponent",
                     []() { return std::make_shared<SpriteComponent>(); });
  register_component("TransformComponent",
                     []() { return std::make_shared<TransformComponent>(); });
  register_component("PlayerMoveComponent",
                     []() { return std::make_shared<PlayerMoveComponent>(); });
  register_component("WeaponComponent",
                     []() { return std::make_shared<WeaponComponent>(); });
}
