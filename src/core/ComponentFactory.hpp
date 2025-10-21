#pragma once
#include "components/IComponent.hpp"
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

class ComponentFactory {
public:
  ComponentFactory();
  ~ComponentFactory() = default;
  using Creator = std::function<std::shared_ptr<Flag::IComponent>()>;

  void register_component(const std::string &name, Creator creator) {
    creators[name] = creator;
  }

  std::shared_ptr<Flag::IComponent> create(const std::string &name) {
    auto it = creators.find(name);
    if (it != creators.end()) {
      return it->second();
    }
    return nullptr;
  }

private:
  std::unordered_map<std::string, Creator> creators;
};
