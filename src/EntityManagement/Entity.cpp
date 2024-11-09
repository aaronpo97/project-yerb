#include "../../includes/EntityManagement/Entity.hpp"
#include <iostream>

bool Entity::isActive() const {
  return m_active;
}

EntityTags Entity::tag() const {
  return m_tag;
}

size_t Entity::id() const {
  return m_id;
}

void Entity::destroy() {
  m_active = false;
}

Vec2 Entity::getCenterPos() const {
  if (cTransform == nullptr || cShape == nullptr) {
    SDL_LogError(
        SDL_LOG_CATEGORY_ERROR,
        "Entity lacks a transform or shape component. Unable to calculate center position.");
    return {0, 0};
  }

  const Vec2 &pos    = cTransform->topLeftCornerPos;
  const Vec2 &center = pos + Vec2(static_cast<float>(cShape->rect.w) / 2.0f,
                                  static_cast<float>(cShape->rect.h) / 2);
  return center;
}
