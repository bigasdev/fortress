#include "GameManager.hpp"

#include "components/IComponent.hpp"
#include "systems/IFixedUpdateSystem.hpp"
#include "systems/IRenderSystem.hpp"
#include "systems/IStartSystem.hpp"
#include "systems/ISystem.hpp"
#include "systems/IUpdateSystem.hpp"

GameManager::GameManager() {
  // component_factory = std::make_unique<ComponentFactory>();
}
