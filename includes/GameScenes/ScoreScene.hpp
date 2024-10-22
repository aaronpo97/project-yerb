#pragma once

#include "../GameEngine/GameEngine.hpp"
#include "../GameScenes/Scene.hpp"

class ScoreScene : public Scene {
private:
  int m_score;

public:
  ScoreScene(GameEngine *gameEngine, int score);

  void update() override;
  void onEnd() override;
  void sRender() override;
  void sDoAction(Action &action) override;
  void renderText();
};
