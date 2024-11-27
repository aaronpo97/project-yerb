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

  Entity(const size_t id, const EntityTags tag) :
      m_id(id), m_tag(tag) {}

public:
  // private member access functions
  bool       isActive() const;
  EntityTags tag() const;
  size_t     id() const;
  void       destroy();
  Vec2       getCenterPos() const;

  template <typename T> std::shared_ptr<T> getComponent() const {
    auto component = std::get<std::shared_ptr<T>>(m_components);
    return component;
  }
  template <typename T> void setComponent(std::shared_ptr<T> component) {
    std::get<std::shared_ptr<T>>(m_components) = component;
  }
  template <typename T> void removeComponent() {
    std::get<std::shared_ptr<T>>(m_components) = nullptr;
  }
};
