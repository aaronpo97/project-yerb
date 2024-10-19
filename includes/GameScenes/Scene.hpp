#pragma once
#include "../GameEngine/Action.hpp"
#include "../GameEngine/GameEngine.hpp"
#include <map>
#include <string>

typedef std::map<int, std::string> ActionMap;
class GameEngine;
class Scene {
protected:
  GameEngine *m_gameEngine;
  Uint64      m_lastFrameTime = 0;
  float       m_deltaTime     = 0;
  bool        m_hasEnded      = false;
  bool        m_paused        = false;
  ActionMap   m_actionMap;

public:
  Scene(GameEngine *gameEngine) :
      m_gameEngine(gameEngine) {};

  virtual ~Scene()                       = default;
  virtual void update()                  = 0;
  virtual void onEnd()                   = 0;
  virtual void sRender()                 = 0;
  virtual void sDoAction(Action &action) = 0;
  void         registerAction(const int inputKey, const std::string &actionName) {
    m_actionMap[inputKey] = actionName;
  }
  void setPaused(const bool paused) {
    m_paused = paused;
  }
  const ActionMap &getActionMap() const {
    return m_actionMap;
  }
};
