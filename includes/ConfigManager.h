#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H
#include "../deps/json.hpp"
#include <fstream>
using json = nlohmann::json;
#include "./Config.h"

class ConfigManager {
private:
  GameConfig   m_gameConfig;
  PlayerConfig m_playerConfig;
  EnemyConfig  m_enemyConfig;

public:
  ConfigManager() {}
  // void loadConfig() {

  //   m_gameConfig   = GameConfig();
  //   m_playerConfig = PlayerConfig();
  //   m_enemyConfig  = EnemyConfig();

  //   // TODO: Load configuration from a file
  //   // Mock Configuration Values
  //   m_gameConfig.windowSize  = Vec2(1366, 768);
  //   m_gameConfig.windowTitle = "C++ SDL2 Window";
  //   m_gameConfig.fontPath    = "./assets/fonts/Sixtyfour/static/Sixtyfour-Regular.ttf";

  //   m_playerConfig.baseSpeed            = 4.0f;
  //   m_playerConfig.speedBoostMultiplier = 2.0f;
  //   m_playerConfig.slownessMultiplier   = 0.8f;
  //   m_playerConfig.shape                = ShapeConfig{80, 80, SDL_Color{0, 0, 255, 255}};

  //   m_enemyConfig.speed = 2.0f;
  //   m_enemyConfig.shape = ShapeConfig{40, 40, SDL_Color{255, 0, 0, 255}};
  // }

  void loadConfig() {
    std::ifstream configFile("./assets/config.json"); // Open your JSON config file
    if (configFile.is_open()) {
      json j;
      configFile >> j; // Parse the JSON file into the json object

      // Access gameConfig
      m_gameConfig.windowSize  = Vec2(j["gameConfig"]["windowSize"]["width"],
                                      j["gameConfig"]["windowSize"]["height"]);
      m_gameConfig.windowTitle = j["gameConfig"]["windowTitle"];
      m_gameConfig.fontPath    = j["gameConfig"]["fontPath"];

      // Access playerConfig
      m_playerConfig.baseSpeed            = j["playerConfig"]["baseSpeed"];
      m_playerConfig.speedBoostMultiplier = j["playerConfig"]["speedBoostMultiplier"];
      m_playerConfig.slownessMultiplier   = j["playerConfig"]["slownessMultiplier"];
      m_playerConfig.shape                = ShapeConfig{j["playerConfig"]["shape"]["width"],
                                         j["playerConfig"]["shape"]["height"],
                                         SDL_Color{j["playerConfig"]["shape"]["color"]["r"],
                                                   j["playerConfig"]["shape"]["color"]["g"],
                                                   j["playerConfig"]["shape"]["color"]["b"],
                                                   j["playerConfig"]["shape"]["color"]["a"]}};

      // Access enemyConfig
      m_enemyConfig.speed = j["enemyConfig"]["speed"];
      m_enemyConfig.shape =
          ShapeConfig{j["enemyConfig"]["shape"]["width"], j["enemyConfig"]["shape"]["height"],
                      SDL_Color{j["enemyConfig"]["shape"]["color"]["r"],
                                j["enemyConfig"]["shape"]["color"]["g"],
                                j["enemyConfig"]["shape"]["color"]["b"],
                                j["enemyConfig"]["shape"]["color"]["a"]}};

      configFile.close(); // Close the file
    } else {
    }
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
  void updatePlayerSpeed(const float speed) {
    m_playerConfig.baseSpeed = speed;
  }

  void updateEnemyShape(const ShapeConfig &shape) {
    m_enemyConfig.shape = shape;
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