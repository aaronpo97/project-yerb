#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H
#include "./Config.h"
#include <fstream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
class ConfigManager {
private:
  GameConfig             m_gameConfig;
  PlayerConfig           m_playerConfig;
  EnemyConfig            m_enemyConfig;
  SpeedBoostEffectConfig m_speedBoostEffectConfig;
  SlownessEffectConfig   m_slownessEffectConfig;
  json                   m_json;
  std::string            m_configPath;

  void parseGameConfig() {
    m_gameConfig.windowSize  = Vec2(m_json["gameConfig"]["windowSize"]["width"],
                                    m_json["gameConfig"]["windowSize"]["height"]);
    m_gameConfig.windowTitle = m_json["gameConfig"]["windowTitle"];
    m_gameConfig.fontPath    = m_json["gameConfig"]["fontPath"];
  }

  void parsePlayerConfig() {
    m_playerConfig.baseSpeed            = m_json["playerConfig"]["baseSpeed"];
    m_playerConfig.speedBoostMultiplier = m_json["playerConfig"]["speedBoostMultiplier"];
    m_playerConfig.slownessMultiplier   = m_json["playerConfig"]["slownessMultiplier"];
    const SDL_Color playerColor = SDL_Color{m_json["playerConfig"]["shape"]["color"]["r"],
                                            m_json["playerConfig"]["shape"]["color"]["g"],
                                            m_json["playerConfig"]["shape"]["color"]["b"],
                                            m_json["playerConfig"]["shape"]["color"]["a"]};
    m_playerConfig.shape        = ShapeConfig{m_json["playerConfig"]["shape"]["height"],
                                       m_json["playerConfig"]["shape"]["width"], playerColor};
  }

  void parseEnemyConfig() {
    m_enemyConfig.speed        = m_json["enemyConfig"]["speed"];
    m_enemyConfig.lifespan     = m_json["enemyConfig"]["lifespan"];
    const SDL_Color enemyColor = SDL_Color{m_json["enemyConfig"]["shape"]["color"]["r"],
                                           m_json["enemyConfig"]["shape"]["color"]["g"],
                                           m_json["enemyConfig"]["shape"]["color"]["b"],
                                           m_json["enemyConfig"]["shape"]["color"]["a"]};
    m_enemyConfig.shape        = ShapeConfig{m_json["enemyConfig"]["shape"]["height"],
                                      m_json["enemyConfig"]["shape"]["width"], enemyColor};
  }

  void parseSpeedBoostEffectConfig() {
    m_speedBoostEffectConfig.speed    = m_json["speedBoostEffectConfig"]["speed"];
    m_speedBoostEffectConfig.lifespan = m_json["speedBoostEffectConfig"]["lifespan"];
    const SDL_Color speedBoostColor =
        SDL_Color{m_json["speedBoostEffectConfig"]["shape"]["color"]["r"],
                  m_json["speedBoostEffectConfig"]["shape"]["color"]["g"],
                  m_json["speedBoostEffectConfig"]["shape"]["color"]["b"],
                  m_json["speedBoostEffectConfig"]["shape"]["color"]["a"]};
    m_speedBoostEffectConfig.shape =
        ShapeConfig{m_json["speedBoostEffectConfig"]["shape"]["height"],
                    m_json["speedBoostEffectConfig"]["shape"]["width"], speedBoostColor};
  }

  void parseSlownessEffectConfig() {
    m_slownessEffectConfig.speed    = m_json["slownessEffectConfig"]["speed"];
    m_slownessEffectConfig.lifespan = m_json["slownessEffectConfig"]["lifespan"];
    const SDL_Color slownessColor =
        SDL_Color{m_json["slownessEffectConfig"]["shape"]["color"]["r"],
                  m_json["slownessEffectConfig"]["shape"]["color"]["g"],
                  m_json["slownessEffectConfig"]["shape"]["color"]["b"],
                  m_json["slownessEffectConfig"]["shape"]["color"]["a"]};
    m_slownessEffectConfig.shape =
        ShapeConfig{m_json["slownessEffectConfig"]["shape"]["height"],
                    m_json["slownessEffectConfig"]["shape"]["width"], slownessColor};
  }

  void parseConfig() {
    parseGameConfig();
    parsePlayerConfig();
    parseEnemyConfig();
    parseSpeedBoostEffectConfig();
    parseSlownessEffectConfig();
  }

  void loadConfig() {
    std::ifstream configFile("./assets/config.json");
    if (!configFile.is_open()) {
      throw std::runtime_error("Could not open config file");
    }
    configFile >> m_json;
    parseConfig();
    configFile.close();
  }

public:
  ConfigManager(std::string configPath = "./assets/config.json") :
      m_configPath(configPath) {
    loadConfig();
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
  const SpeedBoostEffectConfig &getSpeedBoostEffectConfig() const {
    return m_speedBoostEffectConfig;
  }
  const SlownessEffectConfig &getSlownessEffectConfig() const {
    return m_slownessEffectConfig;
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
  void updateSpeedBoostEffectSpeed(const float speed) {
    m_speedBoostEffectConfig.speed = speed;
  }
  void updateSlownessEffectSpeed(const float speed) {
    m_slownessEffectConfig.speed = speed;
  }
};
#endif // CONFIG_MANAGER_H
