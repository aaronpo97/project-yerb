
#include "../../includes/EntityManagement/EntityManager.hpp"
#include "../../includes/EntityManagement/Entity.hpp"
#include <ranges> // For std::ranges::views

EntityManager::EntityManager() = default;

std::shared_ptr<Entity> EntityManager::addEntity(const EntityTags tag) {
  auto entityToAdd = std::shared_ptr<Entity>(new Entity(m_totalEntities++, tag));
  m_toAdd.push_back(entityToAdd);
  return entityToAdd;
}

EntityVector &EntityManager::getEntities() {
  return m_entities;
}
EntityVector &EntityManager::getEntities(const EntityTags tag) {
  return m_entityMap[tag];
}

void EntityManager::update() {
  auto removeDeadEntities = [](EntityVector &entityVec) {
    std::erase_if(entityVec, [](auto &entity) { return !entity->isActive(); });
  };

  // add all entities in the `m_toAdd` vector to the main entity vector
  for (const std::shared_ptr<Entity> &entity : m_toAdd) {
    m_entities.push_back(entity);
    m_entityMap[entity->tag()].push_back(entity);
  }

  // Remove dead entities from the vector of all entities
  removeDeadEntities(m_entities);
  // Remove dead entities from each vector in the entity map
  for (auto &entityVec : m_entityMap | std::views::values) {
    removeDeadEntities(entityVec);
  }

  m_toAdd.clear();
}
