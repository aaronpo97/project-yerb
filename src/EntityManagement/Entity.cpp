#include "../../includes/EntityManagement/Entity.hpp"
#include <iostream>

bool Entity::isActive() const {
  return m_active;
}

const EntityTags Entity::tag() const {
  return m_tag;
}

size_t Entity::id() const {
  return m_id;
}

void Entity::destroy() {
  m_active = false;
}
