#include "../../includes/GameScenes/MenuScene.hpp"
#include "../../includes/GameScenes/HowToPlayScene.hpp"
#include "../../includes/GameScenes/MainScene.hpp"
#include "../../includes/Helpers/TextHelpers.hpp"

#include <SDL2/SDL.h>

MenuScene::MenuScene(GameEngine *gameEngine) :
    Scene(gameEngine) {
  m_selectedIndex = 0;
  registerAction(SDLK_RETURN, "SELECT");
  registerAction(SDLK_w, "UP");
  registerAction(SDLK_s, "DOWN");
}

void MenuScene::update() {
  sRender();
  sAudio();

  if (m_endTriggered) {
    onEnd();
  }
}

void MenuScene::onEnd() {
  switch (m_selectedIndex) {
    case 0:
      m_gameEngine->loadScene("Main", std::make_shared<MainScene>(m_gameEngine));
      break;
    case 1:
    default:
      m_gameEngine->loadScene("HowToPlay", std::make_shared<HowToPlayScene>(m_gameEngine));
      break;
  }
}

void MenuScene::sRender() {
  SDL_Renderer *renderer = m_gameEngine->getRenderer();
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);
  renderText();
  SDL_RenderPresent(renderer);
}

void MenuScene::renderText() const {
  SDL_Renderer *renderer      = m_gameEngine->getRenderer();
  TTF_Font     *fontLg        = m_gameEngine->getFontManager().getFontLg();
  TTF_Font     *fontMd        = m_gameEngine->getFontManager().getFontMd();
  TTF_Font     *fontSm        = m_gameEngine->getFontManager().getFontSm();
  SDL_Color     textColor     = {255, 255, 255, 255};
  SDL_Color     selectedColor = {0, 255, 0, 255};

  if (fontLg == nullptr || fontMd == nullptr || fontSm == nullptr) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to load fonts as they are null.");
    return;
  }

  const std::string titleText = "Yerb's Game";
  const Vec2        titlePos  = {100, 100};
  TextHelpers::renderLineOfText(renderer, fontLg, titleText, textColor, titlePos);

  const std::string playText  = "Play";
  const Vec2        playPos   = {100, 200};
  const SDL_Color   playColor = m_selectedIndex == 0 ? selectedColor : textColor;
  TextHelpers::renderLineOfText(renderer, fontMd, playText, playColor, playPos);

  const std::string instructionsText  = "How to Play";
  const Vec2        instructionsPos   = {100, 250};
  const SDL_Color   instructionsColor = m_selectedIndex == 1 ? selectedColor : textColor;
  TextHelpers::renderLineOfText(renderer, fontMd, instructionsText, instructionsColor,
                                instructionsPos);

  const std::string controlsText = "W/S to move up/down, Enter to select";
  // bottom right corner
  const Vec2 controlsPos = {
      100, m_gameEngine->getConfigManager().getGameConfig().windowSize.y - 50};
  TextHelpers::renderLineOfText(renderer, fontSm, controlsText, textColor, controlsPos);
}

void MenuScene::sDoAction(Action &action) {
  if (action.getState() == ActionState::END) {
    return;
  }

  if (action.getName() == "SELECT") {
    m_nextAudioSample = AudioSample::MENU_SELECT;
    m_endTriggered    = true;
    return;
  }

  // UP takes precedence over DOWN if both are pressed
  if (action.getName() == "UP") {
    m_nextAudioSample = AudioSample::MENU_MOVE;
    m_selectedIndex > 0 ? m_selectedIndex -= 1 : m_selectedIndex = 1;
    return;
  }

  if (action.getName() == "DOWN") {
    m_nextAudioSample = AudioSample::MENU_MOVE;
    m_selectedIndex < 1 ? m_selectedIndex += 1 : m_selectedIndex = 0;
  }
}

void MenuScene::sAudio() {
  AudioManager &audioManager = m_gameEngine->getAudioManager();
  // TEMPORARY FOR NOW
  audioManager.muteTracks();

  if (audioManager.getCurrentAudioTrack() != AudioTrack::MainMenu) {
    m_gameEngine->getAudioManager().playTrack(AudioTrack::MainMenu, -1);
  }

  if (m_nextAudioSample != AudioSample::NONE) {
    audioManager.playSample(m_nextAudioSample);
    m_nextAudioSample = AudioSample::NONE;
  }
}
