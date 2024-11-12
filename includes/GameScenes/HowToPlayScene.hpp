#pragma once

#include "../GameEngine/GameEngine.hpp"
#include "./Scene.hpp"

class HowToPlayScene final : public Scene {
private:
  void renderText() const;

public:
  explicit HowToPlayScene(GameEngine *gameEngine);

  void update() override;
  void onEnd() override;
  void sRender() override;
  void sDoAction(Action &action) override;
  void sAudio() override;
};
