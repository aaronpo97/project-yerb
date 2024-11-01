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

void MenuScene::renderText() {
  SDL_Renderer *renderer      = m_gameEngine->getRenderer();
  TTF_Font     *fontLg        = m_gameEngine->getFontManager().getFontLg();
  TTF_Font     *fontMd        = m_gameEngine->getFontManager().getFontMd();
  SDL_Color     textColor     = {255, 255, 255, 255};
  SDL_Color     selectedColor = {0, 255, 0, 255};

  if (fontLg == nullptr || fontMd == nullptr) {
    std::cerr << "Failed to load font" << std::endl;
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
}

void MenuScene::sDoAction(Action &action) {
  if (action.getState() == ActionState::END) {
    return;
  }

  if (action.getName() == "SELECT") {
    onEnd();
    return;
  }

  // UP takes precedence over DOWN if both are pressed
  if (action.getName() == "UP") {
    m_selectedIndex > 0 ? m_selectedIndex -= 1 : m_selectedIndex = 1;
    return;
  }

  if (action.getName() == "DOWN") {
    m_selectedIndex < 1 ? m_selectedIndex += 1 : m_selectedIndex = 0;
    return;
  }
}
