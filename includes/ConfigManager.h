#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include "./Config.h"

class ConfigManager {
private:
  GameConfig   m_gameConfig;
  PlayerConfig m_playerConfig;
  EnemyConfig  m_enemyConfig;

public:
  ConfigManager() {}
  void loadConfig() {
    // TODO: Load configuration from a file
    // Mock Configuration Values
    m_gameConfig.windowSize  = Vec2(1366, 768);
    m_gameConfig.windowTitle = "C++ SDL2 Window";
    m_gameConfig.fontPath    = "./assets/fonts/Sixtyfour/static/Sixtyfour-Regular.ttf";

    m_playerConfig.speed = 4.0f;
    m_playerConfig.shape = ShapeConfig{80, 80, SDL_Color{0, 0, 255, 255}};

    m_enemyConfig.speed = 2.0f;
    m_enemyConfig.shape = ShapeConfig{40, 40, SDL_Color{255, 0, 0, 255}};
  }

  const GameConfig &getGameConfig() const {
    return m_gameConfig;
  }

  const PlayerConfig &getPlayerConfig() const {
    return m_playerConfig;
  }

  const EnemyConfig &getEnemyConfig() const {
    return m_enemyConfig;
  }

  void updatePlayerShape(const ShapeConfig &shape) {
    m_playerConfig.shape = shape;
  }

  void updateEnemyShape(const ShapeConfig &shape) {
    m_enemyConfig.shape = shape;
  }

  void updatePlayerSpeed(const float speed) {
    m_playerConfig.speed = speed;
  }

  void updateEnemySpeed(const float speed) {
    m_enemyConfig.speed = speed;
  }

  void updateGameWindowSize(const Vec2 &size) {
    m_gameConfig.windowSize = size;
  }

  void updateGameWindowTitle(const std::string &title) {
    m_gameConfig.windowTitle = title;
  }
};

#endif // CONFIG_MANAGER_H