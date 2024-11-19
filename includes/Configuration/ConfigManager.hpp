#pragma once

#include "./Config.hpp"
#include <filesystem>
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

  template <typename JsonReturnType>
  static JsonReturnType
  getJsonValue(const json &jsonValue, const std::string &key, const std::string &context);

  static SDL_Color   parseColor(const json &colorJson, const std::string &context);
  static ShapeConfig parseShapeConfig(const json &shapeJson, const std::string &context);
  void               parseGameConfig();
  void               parseItemConfig();
  void               parsePlayerConfig();
  void               parseEnemyConfig();
  void               parseSpeedBoostEffectConfig();
  void               parseSlownessEffectConfig();
  void               parseBulletConfig();
  void               parseConfig();
  void               loadConfig();

public:
  explicit ConfigManager(std::filesystem::path configPath = "assets/config.json");

  const GameConfig             &getGameConfig() const;
  const ItemConfig             &getItemConfig() const;
  const PlayerConfig           &getPlayerConfig() const;
  const EnemyConfig            &getEnemyConfig() const;
  const BulletConfig           &getBulletConfig() const;
  const SpeedBoostEffectConfig &getSpeedBoostEffectConfig() const;
  const SlownessEffectConfig   &getSlownessEffectConfig() const;

  void updatePlayerShape(const ShapeConfig &shape);
  void updatePlayerSpeed(float speed);
  void updateEnemyShape(const ShapeConfig &shape);
  void updateEnemySpeed(float speed);
  void updateGameWindowSize(const Vec2 &size);
  void updateGameWindowTitle(const std::string &title);
  void updateSpeedBoostEffectSpeed(float speed);
  void updateSlownessEffectSpeed(float speed);
  void updateBulletSpeed(float speed);
  void updateItemSpeed(float speed);
};
