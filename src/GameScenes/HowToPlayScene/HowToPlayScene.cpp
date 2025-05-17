#include "../../../includes/GameScenes/HowToPlayScene/HowToPlayScene.hpp"
#include "../../../includes/GameScenes/MenuScene/MenuScene.hpp"
#include "../../../includes/Helpers/TextHelpers.hpp"
#include <SDL2/SDL.h>

HowToPlayScene::HowToPlayScene(GameEngine *gameEngine) :
    Scene(gameEngine) {
  registerAction(SDLK_RETURN, "SELECT");
  registerAction(SDLK_BACKSPACE, "GO_BACK");
}

void HowToPlayScene::update() {
  sRender();
  sAudio();

  if (m_endTriggered) {
    onEnd();
  }
}

void HowToPlayScene::onEnd() {
  m_gameEngine->loadScene("Menu", std::make_shared<MenuScene>(m_gameEngine));
}

void HowToPlayScene::sRender() {
  SDL_Renderer *renderer = m_gameEngine->getVideoManager().getRenderer();
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);
  renderText();
  SDL_RenderPresent(renderer);
}

void HowToPlayScene::renderText() const {
  SDL_Renderer       *renderer  = m_gameEngine->getVideoManager().getRenderer();
  TTF_Font           *fontSm    = m_gameEngine->getFontManager().getFontSm();
  TTF_Font           *fontMd    = m_gameEngine->getFontManager().getFontMd();
  TTF_Font           *fontLg    = m_gameEngine->getFontManager().getFontLg();
  constexpr SDL_Color textColor = {.r = 255, .g = 255, .b = 255, .a = 255};

  const bool fontsLoaded = fontSm != nullptr && fontMd != nullptr && fontLg != nullptr;

  if (!fontsLoaded) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to load fonts as they are null.");
    return;
  }

  // Instructions text
  const std::string titleText = "How to Play";
  const Vec2        titlePos  = {100, 100};
  TextHelpers::renderLineOfText(renderer, fontLg, titleText, textColor, titlePos);

  // Movement instructions
  const std::string controlsText = "Controls";
  const Vec2        controlsPos  = titlePos + Vec2{0, 80};
  TextHelpers::renderLineOfText(renderer, fontMd, controlsText, textColor, controlsPos);

  const std::string wText = "W: Move Up";
  const Vec2        wPos  = controlsPos + Vec2{0, 80};
  TextHelpers::renderLineOfText(renderer, fontSm, wText, textColor, wPos);

  const std::string sText = "S: Move Down";
  const Vec2        sPos  = wPos + Vec2{0, 40};
  TextHelpers::renderLineOfText(renderer, fontSm, sText, textColor, sPos);

  const std::string aText = "A: Move Left";
  const Vec2        aPos  = sPos + Vec2{0, 40};
  TextHelpers::renderLineOfText(renderer, fontSm, aText, textColor, aPos);

  const std::string dText = "D: Move Right";
  const Vec2        dPos  = aPos + Vec2{0, 40};
  TextHelpers::renderLineOfText(renderer, fontSm, dText, textColor, dPos);

  const std::string enterText = "Enter: Select";
  const Vec2        enterPos  = dPos + Vec2{0, 80};
  TextHelpers::renderLineOfText(renderer, fontSm, enterText, textColor, enterPos);

  const std::string backText = "Back: Backspace";
  const Vec2        backPos  = enterPos + Vec2{0, 40};
  TextHelpers::renderLineOfText(renderer, fontSm, backText, textColor, backPos);

  const std::string shootText = "Mouse Click: Shoot";
  const Vec2        shootPos  = backPos + Vec2{0, 40};
  TextHelpers::renderLineOfText(renderer, fontSm, shootText, textColor, shootPos);

  // Objectives text

  const std::string objectivesText = "Objectives";
  const Vec2        objectivesPos  = controlsPos + Vec2{350, 0};
  TextHelpers::renderLineOfText(renderer, fontMd, objectivesText, textColor, objectivesPos);

  const std::string objective1Text = "1. Collect the yellow squares to increase your score.";
  const Vec2        objective1Pos  = objectivesPos + Vec2{0, 80};
  TextHelpers::renderLineOfText(renderer, fontSm, objective1Text, textColor, objective1Pos);

  const std::string objective2Text =
      "2. Avoid the red squares as they will decrease your lives.";
  const Vec2 objective2Pos = objective1Pos + Vec2{0, 40};
  TextHelpers::renderLineOfText(renderer, fontSm, objective2Text, textColor, objective2Pos);

  const std::string objective3Text = "3. Collect the green squares to gain a speed boost.";
  const Vec2        objective3Pos  = objective2Pos + Vec2{0, 40};
  TextHelpers::renderLineOfText(renderer, fontSm, objective3Text, textColor, objective3Pos);

  const std::string objective4Text = "4. Avoid purple squares as they will slow you down.";
  const Vec2        objective4Pos  = objective3Pos + Vec2{0, 40};
  TextHelpers::renderLineOfText(renderer, fontSm, objective4Text, textColor, objective4Pos);

  const std::string objective5Text = "5. Shoot down red squares to increase your score.";
  const Vec2        objective5Pos  = objective4Pos + Vec2{0, 40};
  TextHelpers::renderLineOfText(renderer, fontSm, objective5Text, textColor, objective5Pos);

  const std::string objective6Text = "6. Avoid shooting any squares you want to collect.";
  const Vec2        objective6Pos  = objective5Pos + Vec2{0, 40};
  TextHelpers::renderLineOfText(renderer, fontSm, objective6Text, textColor, objective6Pos);

  // How to exit text
  const std::string exitText   = "Press Backspace to go back to the main menu.";
  const Vec2        windowSize = m_gameEngine->getConfigManager().getGameConfig().windowSize;
  const Vec2        exitPos    = {100, windowSize.y - 50};
  TextHelpers::renderLineOfText(renderer, fontSm, exitText, textColor, exitPos);
}

void HowToPlayScene::sDoAction(Action &action) {
  AudioSampleQueue &audioSampleQueue = m_gameEngine->getAudioSampleQueue();
  if (action.getState() == ActionState::END) {
    return;
  }

  if (action.getName() == "SELECT") {
    audioSampleQueue.queueSample(AudioSample::MENU_SELECT, AudioSamplePriority::BACKGROUND);
    m_endTriggered = true;
  }

  if (action.getName() == "GO_BACK") {
    audioSampleQueue.queueSample(AudioSample::MENU_SELECT, AudioSamplePriority::BACKGROUND);
    m_endTriggered = true;
  }
}

void HowToPlayScene::sAudio() {
  AudioManager     &audioManager     = m_gameEngine->getAudioManager();
  AudioSampleQueue &audioSampleQueue = m_gameEngine->getAudioSampleQueue();
  if (audioManager.getCurrentAudioTrack() != AudioTrack::MAIN_MENU) {
    audioManager.playTrack(AudioTrack::MAIN_MENU, -1);
  }

  audioSampleQueue.update();
}