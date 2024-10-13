#ifndef MOVEMENT_HELPERS_H
#define MOVEMENT_HELPERS_H

#include "../includes/Entity.h"
#include "../includes/EntityTags.h"
#include "../includes/Game.h"
#include <memory>

namespace MovementHelpers {
  void moveEnemies(std::shared_ptr<Entity> &entity, const Uint32 &deltaTime);
  void moveSpeedBoosts(std::shared_ptr<Entity> &entity, const Uint32 &deltaTime);
  void movePlayer(std::shared_ptr<Entity> &entity,
                  const PlayerConfig      &m_playerConfig,
                  const Uint32            &deltaTime);
} // namespace MovementHelpers

#endif // MOVEMENT_HELPERS_H
