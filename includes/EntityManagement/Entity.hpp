#pragma once

#include "./Components.hpp"
#include <memory>
#include <string>

enum EntityTags { Player, Wall, SpeedBoost, SlownessDebuff, Enemy, Bullet, Item, Default };

typedef std::tuple<std::shared_ptr<CTransform>,
                   std::shared_ptr<CShape>,
                   std::shared_ptr<CInput>,
                   std::shared_ptr<CLifespan>,
                   std::shared_ptr<CEffects>,
                   std::shared_ptr<CBounceTracker>>
    EntityComponents;

class Entity {
private:
  friend class EntityManager;
  bool       m_active = true;
  size_t     m_id     = 0;
  EntityTags m_tag    = Default;

  EntityComponents m_components;

  Entity(size_t id, EntityTags tag);

public:
  // private member access functions
  bool       isActive() const;
  EntityTags tag() const;
  size_t     id() const;
  void       destroy();
  Vec2       getCenterPos() const;

  template <typename T> std::shared_ptr<T> getComponent() const;
  template <typename T> void               setComponent(std::shared_ptr<T> component);
  template <typename T> void               removeComponent();
  template <typename T> bool               hasComponent() const;
};

template <typename T> std::shared_ptr<T> Entity::getComponent() const {
  auto component = std::get<std::shared_ptr<T>>(m_components);
  return component;
}

template <typename T> void Entity::setComponent(std::shared_ptr<T> component) {
  std::get<std::shared_ptr<T>>(m_components) = component;
}
template <typename T> void Entity::removeComponent() {
  std::get<std::shared_ptr<T>>(m_components) = nullptr;
}
template <typename T> bool Entity::hasComponent() const {
  return std::get<std::shared_ptr<T>>(m_components) != nullptr;
}
