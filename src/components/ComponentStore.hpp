#pragma once

#include "../entity/Types.hpp"
#include "components/IComponent.hpp"

#include <algorithm>
#include <type_traits>
#include <unordered_map>
#include <vector>
template <typename T> class ComponentStore {
public:
  void add(EntityID entity, T component) {
    m_components[entity] = std::move(component);
  }
  T *get(EntityID entity) {
    auto it = m_components.find(entity);
    return it != m_components.end() ? &it->second : nullptr;
  }
  void remove(EntityID entity) { m_components.erase[entity]; }
  std::vector<T *> get_all() {
    std::vector<T *> result;
    for (auto &pair : m_components) {
      result.push_back(&pair.second);
    }
    return result;
  }

private:
  std::unordered_map<EntityID, T> m_components;
};
