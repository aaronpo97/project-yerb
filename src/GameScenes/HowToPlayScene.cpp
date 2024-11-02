#include "../../includes/GameScenes/HowToPlayScene.hpp"
#include "../../includes/GameScenes/MenuScene.hpp"
#include "../../includes/Helpers/TextHelpers.hpp"
#include <SDL2/SDL.h>
#include <iostream>

HowToPlayScene::HowToPlayScene(GameEngine *gameEngine) :
    Scene(gameEngine) {
  registerAction(SDLK_RETURN, "SELECT");
  registerAction(SDLK_BACKSPACE, "GO_BACK");
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
  SDL_Renderer   *renderer  = m_gameEngine->getRenderer();
  TTF_Font       *fontMd    = m_gameEngine->getFontManager().getFontMd();
  TTF_Font       *fontLg    = m_gameEngine->getFontManager().getFontLg();
  const SDL_Color textColor = {255, 255, 255, 255};

  const bool fontsLoaded = fontMd != nullptr && fontLg != nullptr;

  if (!fontsLoaded) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to load fonts as they are null.");
    return;
  }

  // Instructions text
  const std::string instructionsText = "How to Play";
  const Vec2        instructionsPos  = {100, 100};
  TextHelpers::renderLineOfText(renderer, fontLg, instructionsText, textColor,
                                instructionsPos);

  const std::string wText = "W: Move Up";
  const Vec2        wPos  = {100, 200};
  TextHelpers::renderLineOfText(renderer, fontMd, wText, textColor, wPos);

  const std::string sText = "S: Move Down";
  const Vec2        sPos  = {100, 250};
  TextHelpers::renderLineOfText(renderer, fontMd, sText, textColor, sPos);

  const std::string aText = "A: Move Left";
  const Vec2        aPos  = {100, 300};
  TextHelpers::renderLineOfText(renderer, fontMd, aText, textColor, aPos);

  const std::string dText = "D: Move Right";
  const Vec2        dPos  = {100, 350};
  TextHelpers::renderLineOfText(renderer, fontMd, dText, textColor, dPos);

  const std::string escapeText = "Enter: Select";
  const Vec2        escapePos  = {100, 400};
  TextHelpers::renderLineOfText(renderer, fontMd, escapeText, textColor, escapePos);

  const std::string backText = "Back: Backspace";
  const Vec2        backPos  = {100, 450};
  TextHelpers::renderLineOfText(renderer, fontMd, backText, textColor, backPos);
}

void HowToPlayScene::sDoAction(Action &action) {
  if (action.getState() == ActionState::END) {
    return;
  }

  if (action.getName() == "SELECT") {
    onEnd();
  }

  if (action.getName() == "GO_BACK") {
    onEnd();
  }
}
