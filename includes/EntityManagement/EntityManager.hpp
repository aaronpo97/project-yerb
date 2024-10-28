#pragma once

#include "./Entity.hpp"
#include <map>
#include <memory>
#include <vector>

// Store all entity objects in a vector.
typedef std::vector<std::shared_ptr<Entity>> EntityVector;

// Store separate vectors of Entity objects by their tag for quick retrieval.
typedef std::map<EntityTags, EntityVector> EntityMap;

class EntityManager {
  EntityVector m_entities;
  EntityVector m_toAdd;
  EntityMap    m_entityMap;
  size_t       m_totalEntities = 0;

public:
  EntityManager();
  std::shared_ptr<Entity> addEntity(const EntityTags tag);
  EntityVector           &getEntities();
  EntityVector           &getEntities(const EntityTags tag);
  void                    update();
};
