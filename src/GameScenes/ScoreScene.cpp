#include "../../includes/GameScenes/ScoreScene.hpp"
#include "../../includes/GameScenes/MainScene.hpp"
#include "../../includes/GameScenes/MenuScene.hpp"
#include "../../includes/Helpers/TextHelpers.hpp"
#include <SDL2/SDL.h>
#include <iostream>

ScoreScene::ScoreScene(GameEngine *gameEngine, int score) :
    Scene(gameEngine), m_score(score) {

  registerAction(SDLK_RETURN, "SELECT");
  registerAction(SDLK_w, "UP");
  registerAction(SDLK_s, "DOWN");
}

void ScoreScene::update() {
  sRender();
}

void ScoreScene::onEnd() {
  if (m_selectedIndex == 0) {
    m_gameEngine->loadScene("Main", std::make_shared<MainScene>(m_gameEngine));
  } else if (m_selectedIndex == 1) {
    m_gameEngine->loadScene("Menu", std::make_shared<MenuScene>(m_gameEngine));
  }
}

void ScoreScene::sRender() {
  SDL_Renderer *renderer = m_gameEngine->getRenderer();
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);
  renderText();
  SDL_RenderPresent(renderer);
}

void ScoreScene::renderText() {
  SDL_Renderer   *renderer      = m_gameEngine->getRenderer();
  TTF_Font       *fontLg        = m_gameEngine->getFontLg();
  TTF_Font       *fontMd        = m_gameEngine->getFontMd();
  TTF_Font       *fontSm        = m_gameEngine->getFontSm();
  const SDL_Color gameOverColor = {255, 0, 0, 255};
  const SDL_Color textColor     = {255, 255, 255, 255};
  const SDL_Color selectedColor = {0, 255, 0, 255};

  if (fontMd == nullptr) {
    std::cerr << "Failed to load font" << std::endl;
    return;
  }

  const std::string gameOverText = "Game Over!";
  const Vec2        gameOverPos  = {100, 300};
  TextHelpers::renderLineOfText(renderer, fontLg, gameOverText, gameOverColor, gameOverPos);

  const std::string scoreText = "Score: " + std::to_string(m_score);
  const Vec2        scorePos  = {100, 350};
  TextHelpers::renderLineOfText(renderer, fontMd, scoreText, textColor, scorePos);

  const float bottomOfScreen = m_gameEngine->getConfigManager().getGameConfig().windowSize.y;

  const std::string playAgainText  = "Play Again";
  const Vec2        playAgainPos   = {100, bottomOfScreen - 200};
  const SDL_Color   playAgainColor = m_selectedIndex == 0 ? selectedColor : textColor;
  TextHelpers::renderLineOfText(renderer, fontMd, playAgainText, playAgainColor, playAgainPos);

  const std::string mainMenuText  = "Main Menu";
  const Vec2        mainMenuPos   = {100, bottomOfScreen - 150};
  const SDL_Color   mainMenuColor = m_selectedIndex == 1 ? selectedColor : textColor;
  TextHelpers::renderLineOfText(renderer, fontMd, mainMenuText, mainMenuColor, mainMenuPos);
}

void ScoreScene::sDoAction(Action &action) {
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