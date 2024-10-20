#include "../../includes/GameScenes/MenuScene.hpp"
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
  if (m_selectedIndex == 0) {
    m_gameEngine->switchScene("main");
  } else if (m_selectedIndex == 1) {
    // m_gameEngine->switchScene("InstructionsScene");
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
  SDL_Renderer *renderer  = m_gameEngine->getRenderer();
  TTF_Font     *fontLg    = m_gameEngine->getFontLg();
  TTF_Font     *fontMd    = m_gameEngine->getFontMd();
  SDL_Color     textColor = {255, 255, 255, 255};

  if (fontLg == nullptr || fontMd == nullptr) {
    std::cerr << "Failed to load font" << std::endl;
    return;
  }

  const std::string titleText = "Yerb's Game";
  Vec2              titlePos  = {100, 100};
  TextHelpers::renderLineOfText(renderer, fontLg, titleText, textColor, titlePos);

  const std::string playText = "Play";
  Vec2              playPos  = {100, 200};
  TextHelpers::renderLineOfText(renderer, fontMd, playText, textColor, playPos);

  const std::string instructionsText = "How to Play";
  Vec2              instructionsPos  = {100, 250};
  TextHelpers::renderLineOfText(renderer, fontMd, instructionsText, textColor,
                                instructionsPos);
}

void MenuScene::sDoAction(Action &action) {
  if (action.getName() == "SELECT") {
    onEnd();
  } else if (action.getName() == "UP") {
    if (m_selectedIndex > 0) {
      m_selectedIndex--;
    }
  } else if (action.getName() == "DOWN") {
    if (m_selectedIndex < 1) {
      m_selectedIndex++;
    }
  }
}
