#pragma once
#include "../GameEngine/Action.hpp"
#include "../GameEngine/GameEngine.hpp"
#include "./Scene.hpp"
#include <map>
#include <string>

class MenuScene final : public Scene {
private:
  void         renderText() const;
  bool         m_playButtonClicked         = false;
  bool         m_instructionsButtonClicked = false;
  unsigned int m_selectedIndex             = 0;

public:
  explicit MenuScene(GameEngine *gameEngine);
  void     update() override;
  void     onEnd() override;
  void     sRender() override;
  void     sDoAction(Action &action) override;
  void     sAudio() override;
  void     scaleScene() override{};
};
