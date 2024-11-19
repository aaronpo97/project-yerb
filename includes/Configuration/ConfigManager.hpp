#pragma once

#include "./Config.hpp"
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <stdexcept>

using json   = nlohmann::json;
namespace fs = std::filesystem;

class ConfigurationError : public std::runtime_error {
public:
  explicit ConfigurationError(const std::string &message) :
      std::runtime_error(message) {}
};

class ConfigManager {
private:
  GameConfig             m_gameConfig;
  PlayerConfig           m_playerConfig;
  EnemyConfig            m_enemyConfig;
  BulletConfig           m_bulletConfig;
  ItemConfig             m_itemConfig;
  SpeedBoostEffectConfig m_speedBoostEffectConfig;
  SlownessEffectConfig   m_slownessEffectConfig;
  json                   m_json;
  std::filesystem::path  m_configPath;

  template <typename T>
  T getJsonValue(const json &j, const std::string &key, const std::string &context) const {
    try {
      return j.at(key).get<T>();
    } catch (const json::exception &e) {
      throw ConfigurationError("Error parsing " + context + ": " + key + " - " + e.what());
    }
  }

  SDL_Color parseColor(const json &colorJson, const std::string &context) {
    return SDL_Color{static_cast<Uint8>(getJsonValue<int>(colorJson, "r", context)),
                     static_cast<Uint8>(getJsonValue<int>(colorJson, "g", context)),
                     static_cast<Uint8>(getJsonValue<int>(colorJson, "b", context)),
                     static_cast<Uint8>(getJsonValue<int>(colorJson, "a", context))};
  }

  ShapeConfig parseShapeConfig(const json &shapeJson, const std::string &context) {
    return ShapeConfig{getJsonValue<float>(shapeJson, "height", context),
                       getJsonValue<float>(shapeJson, "width", context),
                       parseColor(shapeJson["color"], context + ".color")};
  }

  void parseGameConfig() {
    const auto &config   = m_json["gameConfig"];
    const auto &sizeJson = config["windowSize"];

    m_gameConfig.windowSize =
        Vec2(getJsonValue<float>(sizeJson, "width", "gameConfig.windowSize"),
             getJsonValue<float>(sizeJson, "height", "gameConfig.windowSize"));

    m_gameConfig.windowTitle = getJsonValue<std::string>(config, "windowTitle", "gameConfig");
    m_gameConfig.fontPath =
        getJsonValue<std::filesystem::path>(config, "fontPath", "gameConfig");
    m_gameConfig.spawnInterval = getJsonValue<Uint64>(config, "spawnInterval", "gameConfig");

    if (!fs::exists(m_gameConfig.fontPath)) {
      throw ConfigurationError("Font file not found: " + m_gameConfig.fontPath.string());
    }
  }

  void parseItemConfig() {
    const auto &config = m_json["itemConfig"];

    m_itemConfig.lifespan = getJsonValue<Uint64>(config, "lifespan", "itemConfig");
    m_itemConfig.speed    = getJsonValue<float>(config, "speed", "itemConfig");
    m_itemConfig.spawnPercentage =
        getJsonValue<unsigned int>(config, "spawnPercentage", "itemConfig");
    m_itemConfig.shape = parseShapeConfig(config["shape"], "itemConfig.shape");

    if (m_itemConfig.spawnPercentage > 100) {
      throw ConfigurationError("Item spawn percentage must be between 0 and 100");
    }
  }

  void parsePlayerConfig() {
    const auto &config = m_json["playerConfig"];

    m_playerConfig.baseSpeed = getJsonValue<float>(config, "baseSpeed", "playerConfig");
    m_playerConfig.speedBoostMultiplier =
        getJsonValue<float>(config, "speedBoostMultiplier", "playerConfig");
    m_playerConfig.slownessMultiplier =
        getJsonValue<float>(config, "slownessMultiplier", "playerConfig");
    m_playerConfig.shape = parseShapeConfig(config["shape"], "playerConfig.shape");

    if (m_playerConfig.speedBoostMultiplier <= 0 || m_playerConfig.slownessMultiplier <= 0) {
      throw ConfigurationError("Player speed multipliers must be positive");
    }
  }

  void parseEnemyConfig() {
    const auto &config = m_json["enemyConfig"];

    m_enemyConfig.speed    = getJsonValue<float>(config, "speed", "enemyConfig");
    m_enemyConfig.lifespan = getJsonValue<Uint64>(config, "lifespan", "enemyConfig");
    m_enemyConfig.spawnPercentage =
        getJsonValue<unsigned int>(config, "spawnPercentage", "enemyConfig");
    m_enemyConfig.shape = parseShapeConfig(config["shape"], "enemyConfig.shape");

    if (m_enemyConfig.spawnPercentage > 100) {
      throw ConfigurationError("Enemy spawn percentage must be between 0 and 100");
    }
  }

  void parseSpeedBoostEffectConfig() {
    const auto &config = m_json["speedBoostEffectConfig"];

    m_speedBoostEffectConfig.speed =
        getJsonValue<float>(config, "speed", "speedBoostEffectConfig");
    m_speedBoostEffectConfig.lifespan =
        getJsonValue<Uint64>(config, "lifespan", "speedBoostEffectConfig");
    m_speedBoostEffectConfig.spawnPercentage =
        getJsonValue<unsigned int>(config, "spawnPercentage", "speedBoostEffectConfig");
    m_speedBoostEffectConfig.shape =
        parseShapeConfig(config["shape"], "speedBoostEffectConfig.shape");

    if (m_speedBoostEffectConfig.spawnPercentage > 100) {
      throw ConfigurationError("SpeedBoost spawn percentage must be between 0 and 100");
    }
  }

  void parseSlownessEffectConfig() {
    const auto &config = m_json["slownessEffectConfig"];

    m_slownessEffectConfig.speed =
        getJsonValue<float>(config, "speed", "slownessEffectConfig");
    m_slownessEffectConfig.lifespan =
        getJsonValue<Uint64>(config, "lifespan", "slownessEffectConfig");
    m_slownessEffectConfig.spawnPercentage =
        getJsonValue<unsigned int>(config, "spawnPercentage", "slownessEffectConfig");
    m_slownessEffectConfig.shape =
        parseShapeConfig(config["shape"], "slownessEffectConfig.shape");

    if (m_slownessEffectConfig.spawnPercentage > 100) {
      throw ConfigurationError("Slowness spawn percentage must be between 0 and 100");
    }
  }

  void parseBulletConfig() {
    const auto &config = m_json["bulletConfig"];

    m_bulletConfig.speed    = getJsonValue<float>(config, "speed", "bulletConfig");
    m_bulletConfig.lifespan = getJsonValue<Uint64>(config, "lifespan", "bulletConfig");
    m_bulletConfig.shape    = parseShapeConfig(config["shape"], "bulletConfig.shape");
  }

  void parseConfig() {
    try {
      parseGameConfig();
      parsePlayerConfig();
      parseEnemyConfig();
      parseItemConfig();
      parseBulletConfig();
      parseSpeedBoostEffectConfig();
      parseSlownessEffectConfig();
    } catch (const json::exception &e) {
      throw ConfigurationError("JSON parsing error: " + std::string(e.what()));
    }
  }

  void loadConfig() {
    if (!fs::exists(m_configPath)) {
      throw ConfigurationError("Config file not found: " + m_configPath.string());
    }

    std::ifstream configFile(m_configPath);
    if (!configFile.is_open()) {
      throw ConfigurationError("Failed to open config file: " + m_configPath.string());
    }

    try {
      configFile >> m_json;
    } catch (const json::parse_error &e) {
      throw ConfigurationError("JSON parse error: " + std::string(e.what()));
    }

    parseConfig();
    configFile.close();
  }

public:
  explicit ConfigManager(std::filesystem::path configPath = "./assets/config.json") :
      m_configPath(std::move(configPath)) {
    try {
      loadConfig();
      SDL_LogInfo(SDL_LOG_CATEGORY_SYSTEM, "ConfigManager successfully loaded: %s",
                  m_configPath.c_str());
    } catch (const ConfigurationError &e) {
      SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Configuration error: %s", e.what());
      std::cerr << e.what() << std::endl;
      throw e;
    }
  }

  const GameConfig &getGameConfig() const {
    return m_gameConfig;
  }
  const ItemConfig &getItemConfig() const {
    return m_itemConfig;
  }
  const PlayerConfig &getPlayerConfig() const {
    return m_playerConfig;
  }
  const EnemyConfig &getEnemyConfig() const {
    return m_enemyConfig;
  }
  const BulletConfig &getBulletConfig() const {
    return m_bulletConfig;
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
    if (speed <= 0) {
      throw ConfigurationError("Player speed must be positive");
    }
    m_playerConfig.baseSpeed = speed;
  }

  void updateEnemyShape(const ShapeConfig &shape) {
    m_enemyConfig.shape = shape;
  }

  void updateEnemySpeed(const float speed) {
    if (speed <= 0) {
      throw ConfigurationError("Enemy speed must be positive");
    }
    m_enemyConfig.speed = speed;
  }

  void updateGameWindowSize(const Vec2 &size) {
    if (size.x <= 0 || size.y <= 0) {
      throw ConfigurationError("Window dimensions must be positive");
    }
    m_gameConfig.windowSize = size;
  }

  void updateGameWindowTitle(const std::string &title) {
    if (title.empty()) {
      throw ConfigurationError("Window title cannot be empty");
    }
    m_gameConfig.windowTitle = title;
  }

  void updateSpeedBoostEffectSpeed(const float speed) {
    if (speed <= 0) {
      throw ConfigurationError("Speed boost effect speed must be positive");
    }
    m_speedBoostEffectConfig.speed = speed;
  }

  void updateSlownessEffectSpeed(const float speed) {
    if (speed <= 0) {
      throw ConfigurationError("Slowness effect speed must be positive");
    }
    m_slownessEffectConfig.speed = speed;
  }

  void updateBulletSpeed(const float speed) {
    if (speed <= 0) {
      throw ConfigurationError("Bullet speed must be positive");
    }
    m_bulletConfig.speed = speed;
  }

  void updateItemSpeed(const float speed) {
    if (speed <= 0) {
      throw ConfigurationError("Item speed must be positive");
    }
    m_itemConfig.speed = speed;
  }
};