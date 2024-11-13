#include "../../includes/EntityManagement/EntityManager.hpp"
#include "gtest/gtest.h"

class EntityManagerTest : public ::testing::Test {
protected:
  EntityManager manager;

  void SetUp() override {
    // Setup runs before each test
    std::cout << "Setting up EntityManagerTest" << std::endl;
  }
};

// Test entity creation and addition
TEST_F(EntityManagerTest, AddEntity) {
  auto entity = manager.addEntity(EntityTags::Player);

  // Entity should be created with correct properties
  EXPECT_EQ(entity->tag(), EntityTags::Player);
  EXPECT_EQ(entity->id(), 0); // First entity should have ID 0

  // Entity shouldn't be in main vector until update
  EXPECT_TRUE(manager.getEntities().empty());

  manager.update();

  // After update, entity should be in main vector
  EXPECT_EQ(manager.getEntities().size(), 1);
  EXPECT_EQ(manager.getEntities()[0], entity);
}

// Test getting entities by tag
TEST_F(EntityManagerTest, GetEntitiesByTag) {
  auto player = manager.addEntity(EntityTags::Player);
  auto enemy1 = manager.addEntity(EntityTags::Enemy);
  auto enemy2 = manager.addEntity(EntityTags::Enemy);

  manager.update();

  // Check player entities
  auto &players = manager.getEntities(EntityTags::Player);
  EXPECT_EQ(players.size(), 1);
  EXPECT_EQ(players[0], player);

  // Check enemy entities
  auto &enemies = manager.getEntities(EntityTags::Enemy);
  EXPECT_EQ(enemies.size(), 2);
  EXPECT_TRUE(std::find(enemies.begin(), enemies.end(), enemy1) != enemies.end());
  EXPECT_TRUE(std::find(enemies.begin(), enemies.end(), enemy2) != enemies.end());
}

// Test entity ID assignment
TEST_F(EntityManagerTest, EntityIDAssignment) {
  auto entity1 = manager.addEntity(EntityTags::Player);
  auto entity2 = manager.addEntity(EntityTags::Enemy);
  auto entity3 = manager.addEntity(EntityTags::Player);

  EXPECT_EQ(entity1->id(), 0);
  EXPECT_EQ(entity2->id(), 1);
  EXPECT_EQ(entity3->id(), 2);
}

// Test entity removal when inactive
TEST_F(EntityManagerTest, RemoveInactiveEntities) {
  auto entity1 = manager.addEntity(EntityTags::Player);
  auto entity2 = manager.addEntity(EntityTags::Enemy);
  auto entity3 = manager.addEntity(EntityTags::Enemy);

  manager.update();
  EXPECT_EQ(manager.getEntities().size(), 3);

  // Deactivate one enemy
  entity2->destroy();

  manager.update();

  // Check main entity vector
  EXPECT_EQ(manager.getEntities().size(), 2);
  EXPECT_TRUE(std::find(manager.getEntities().begin(), manager.getEntities().end(), entity2) ==
              manager.getEntities().end());

  // Check tag-specific vector
  auto &enemies = manager.getEntities(EntityTags::Enemy);
  EXPECT_EQ(enemies.size(), 1);
  EXPECT_EQ(enemies[0], entity3);
}

// Test multiple updates
TEST_F(EntityManagerTest, MultipleUpdates) {
  auto entity1 = manager.addEntity(EntityTags::Player);
  manager.update();

  auto entity2 = manager.addEntity(EntityTags::Enemy);
  manager.update();

  EXPECT_EQ(manager.getEntities().size(), 2);
  EXPECT_EQ(manager.getEntities(EntityTags::Player).size(), 1);
  EXPECT_EQ(manager.getEntities(EntityTags::Enemy).size(), 1);
}

// Test edge case: deactivating all entities
TEST_F(EntityManagerTest, DeactivateAllEntities) {
  auto entity1 = manager.addEntity(EntityTags::Player);
  auto entity2 = manager.addEntity(EntityTags::Enemy);
  auto entity3 = manager.addEntity(EntityTags::Enemy);

  manager.update();

  // Deactivate all entities
  entity1->destroy();
  entity2->destroy();
  entity3->destroy();

  manager.update();

  // All vectors should be empty
  EXPECT_TRUE(manager.getEntities().empty());
  EXPECT_TRUE(manager.getEntities(EntityTags::Player).empty());
  EXPECT_TRUE(manager.getEntities(EntityTags::Enemy).empty());
}

// Test adding entities of mixed types
TEST_F(EntityManagerTest, MixedEntityTypes) {
  auto player1 = manager.addEntity(EntityTags::Player);
  auto enemy1  = manager.addEntity(EntityTags::Enemy);
  auto player2 = manager.addEntity(EntityTags::Player);
  auto enemy2  = manager.addEntity(EntityTags::Enemy);

  manager.update();

  EXPECT_EQ(manager.getEntities().size(), 4);
  EXPECT_EQ(manager.getEntities(EntityTags::Player).size(), 2);
  EXPECT_EQ(manager.getEntities(EntityTags::Enemy).size(), 2);
}