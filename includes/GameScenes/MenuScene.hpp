#pragma once
#include "../GameEngine/Action.hpp"
#include "../GameEngine/GameEngine.hpp"
#include "./Scene.hpp"
#include <map>
#include <string>

class MenuScene : public Scene {
private:
  void renderText();
  bool m_playButtonClicked         = false;
  bool m_instructionsButtonClicked = false;
  int  m_selectedIndex             = 0;

public:
  MenuScene(GameEngine *gameEngine);
  virtual void update() override;
  virtual void onEnd() override;
  virtual void sRender() override;
  virtual void sDoAction(Action &action) override;
};
