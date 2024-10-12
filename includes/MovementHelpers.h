#ifndef MOVEMENT_HELPERS_H
#define MOVEMENT_HELPERS_H

#include "../includes/Entity.h"
#include "../includes/EntityTags.h"
#include "../includes/Game.h"
#include <memory>

namespace MovementHelpers {
  void moveEnemies(std::shared_ptr<Entity> &entity);
  void moveSpeedBoosts(std::shared_ptr<Entity> &entity);
  void movePlayer(std::shared_ptr<Entity> &entity, const PlayerConfig &m_playerConfig);
} // namespace MovementHelpers

#endif // MOVEMENT_HELPERS_H
