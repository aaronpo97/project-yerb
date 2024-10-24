#pragma once

#include "../GameEngine/GameEngine.hpp"
#include "../GameScenes/Scene.hpp"

class ScoreScene : public Scene {
private:
  unsigned int m_score;
  unsigned int m_selectedIndex = 0;
  void         renderText();

public:
  ScoreScene(GameEngine *gameEngine, int score);

  void update() override;
  void onEnd() override;
  void sRender() override;
  void sDoAction(Action &action) override;
};
