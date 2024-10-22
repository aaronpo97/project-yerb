#include "../../includes/GameScenes/HowToPlayScene.hpp"
#include "../../includes/GameScenes/MenuScene.hpp"
#include "../../includes/Helpers/TextHelpers.hpp"
#include <SDL2/SDL.h>
#include <iostream>

HowToPlayScene::HowToPlayScene(GameEngine *gameEngine) :
    Scene(gameEngine) {

  // TODO: Instead of space, the user should be able to press Enter to go back. Currently
  // unable to do this as the enter key bleeds into the next scene and triggers the select
  // action.
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
  SDL_Color     textColor = {255, 255, 255, 255};

  if (fontMd == nullptr) {
    std::cerr << "Failed to load font" << std::endl;
    return;
  }

  // Instructions text
  std::string instructionsText = "How to Play";
  Vec2        instructionsPos  = {100, 100};
  TextHelpers::renderLineOfText(renderer, fontMd, instructionsText, textColor,
                                instructionsPos);

  const std::string wText = "W: Move Up";
  const Vec2        wPos  = {100, 150};
  TextHelpers::renderLineOfText(renderer, fontMd, wText, textColor, wPos);

  const std::string sText = "S: Move Down";
  const Vec2        sPos  = {100, 200};
  TextHelpers::renderLineOfText(renderer, fontMd, sText, textColor, sPos);

  std::string aText = "A: Move Left";
  Vec2        aPos  = {100, 250};
  TextHelpers::renderLineOfText(renderer, fontMd, aText, textColor, aPos);

  std::string dText = "D: Move Right";
  Vec2        dPos  = {100, 300};
  TextHelpers::renderLineOfText(renderer, fontMd, dText, textColor, dPos);

  std::string escapeText = "Enter: Select";
  Vec2        escapePos  = {100, 350};
  TextHelpers::renderLineOfText(renderer, fontMd, escapeText, textColor, escapePos);

  std::string backText = "To go back to the main menu, press the Enter key.";
  Vec2        backPos  = {100, 400};
  TextHelpers::renderLineOfText(renderer, fontMd, backText, textColor, backPos);
}

void HowToPlayScene::sDoAction(Action &action) {
  if (action.getName() == "SELECT" && action.getState() == ActionState::START) {
    onEnd();
  }
}
