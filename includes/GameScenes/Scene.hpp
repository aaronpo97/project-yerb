#pragma once
#include "../GameEngine/Action.hpp"
#include "../GameEngine/GameEngine.hpp"
#include <map>
#include <string>

typedef std::map<int, std::string> ActionMap;
class GameEngine; // Resolve circular dependency with forward declaration
class Scene {
protected:
  GameEngine *m_gameEngine;
  Uint64      m_lastFrameTime  = 0;
  float       m_deltaTime      = 0;
  bool        m_endTriggered   = false;
  bool        m_hasEnded       = false;
  bool        m_paused         = false;
  Uint64      m_SceneStartTime = 0;
  ActionMap   m_actionMap;

public:
  explicit Scene(GameEngine *gameEngine) :
      m_gameEngine(gameEngine){};

  virtual ~    Scene()                   = default;
  virtual void update()                  = 0;
  virtual void onEnd()                   = 0;
  virtual void sRender()                 = 0;
  virtual void sDoAction(Action &action) = 0;
  virtual void sAudio()                  = 0;
  virtual void scaleScene()              = 0;

  void registerAction(const int inputKey, const std::string &actionName) {
    m_actionMap[inputKey] = actionName;
  }
  void setPaused(const bool paused) {
    m_paused = paused;
  }
  const ActionMap &getActionMap() const {
    return m_actionMap;
  }
  void setStartTime(const Uint64 startTime) {
    m_SceneStartTime = startTime;
  }
  const Uint64 &getStartTime() const {
    return m_SceneStartTime;
  }
};
