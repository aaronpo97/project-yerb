#pragma once

#include "./Components.hpp"
#include <memory>
#include <string>

enum EntityTags { Player, Wall, SpeedBoost, SlownessDebuff, Enemy, Default, Bullet };

class Entity {
private:
  friend class EntityManager;
  bool       m_active = true;
  size_t     m_id     = 0;
  EntityTags m_tag    = Default;

  Entity(const size_t id, const EntityTags tag) :
      m_id(id), m_tag(tag) {}

public:
  // component pointers
  std::shared_ptr<CTransform> cTransform;
  std::shared_ptr<CShape>     cShape;
  std::shared_ptr<CInput>     cInput;
  std::shared_ptr<CLifespan>  cLifespan;
  std::shared_ptr<CEffects>   cEffects;

  // private member access functions
  bool       isActive() const;
  EntityTags tag() const;
  size_t     id() const;
  void       destroy();
};
