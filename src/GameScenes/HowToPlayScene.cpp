#include "../../includes/GameScenes/HowToPlayScene.hpp"
#include "../../includes/GameScenes/MenuScene.hpp"
#include "../../includes/Helpers/TextHelpers.hpp"
#include <SDL2/SDL.h>
#include <iostream>

HowToPlayScene::HowToPlayScene(GameEngine *gameEngine) :
    Scene(gameEngine) {
  registerAction(SDLK_RETURN, "SELECT");
}

void HowToPlayScene::update() {
  sRender();
}

void HowToPlayScene::onEnd() {
  m_gameEngine->loadScene("Menu", std::make_shared<MenuScene>(m_gameEngine));
}

void HowToPlayScene::sRender() {
  SDL_Renderer *renderer = m_gameEngine->getRenderer();
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);
  renderText();
  SDL_RenderPresent(renderer);
}

void HowToPlayScene::renderText() {
  SDL_Renderer *renderer  = m_gameEngine->getRenderer();
  TTF_Font     *fontMd    = m_gameEngine->getFontMd();
  TTF_Font     *fontLg    = m_gameEngine->getFontLg();
  TTF_Font     *fontSm    = m_gameEngine->getFontSm();
  SDL_Color     textColor = {255, 255, 255, 255};

  if (fontMd == nullptr) {
    std::cerr << "Failed to load font" << std::endl;
    return;
  }

  // Instructions text
  std::string instructionsText = "How to Play";
  Vec2        instructionsPos  = {100, 100};
  TextHelpers::renderLineOfText(renderer, fontLg, instructionsText, textColor,
                                instructionsPos);

  const std::string wText = "W: Move Up";
  const Vec2        wPos  = {100, 200};
  TextHelpers::renderLineOfText(renderer, fontMd, wText, textColor, wPos);

  const std::string sText = "S: Move Down";
  const Vec2        sPos  = {100, 250};
  TextHelpers::renderLineOfText(renderer, fontMd, sText, textColor, sPos);

  std::string aText = "A: Move Left";
  Vec2        aPos  = {100, 300};
  TextHelpers::renderLineOfText(renderer, fontMd, aText, textColor, aPos);

  std::string dText = "D: Move Right";
  Vec2        dPos  = {100, 350};
  TextHelpers::renderLineOfText(renderer, fontMd, dText, textColor, dPos);

  std::string escapeText = "Enter: Select";
  Vec2        escapePos  = {100, 400};
  TextHelpers::renderLineOfText(renderer, fontMd, escapeText, textColor, escapePos);

  // bottom of the screen
  const float bottomOfScreen = m_gameEngine->getConfigManager().getGameConfig().windowSize.y;
  std::string backText       = "To go back to the main menu, press the Enter key.";
  Vec2        backPos        = {100, bottomOfScreen - 50};
  TextHelpers::renderLineOfText(renderer, fontSm, backText, textColor, backPos);
}

void HowToPlayScene::sDoAction(Action &action) {

  if (action.getState() == ActionState::END) {
    return;
  }

  if (action.getName() == "SELECT") {
    onEnd();
  }
}
