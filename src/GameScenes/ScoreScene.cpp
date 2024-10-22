#include "../../includes/GameScenes/ScoreScene.hpp"
#include "../../includes/GameScenes/MenuScene.hpp"
#include "../../includes/Helpers/TextHelpers.hpp"
#include <SDL2/SDL.h>
#include <iostream>

ScoreScene::ScoreScene(GameEngine *gameEngine, int score) :
    Scene(gameEngine), m_score(score) {

  registerAction(SDLK_RETURN, "SELECT");
}

void ScoreScene::update() {
  sRender();
}

void ScoreScene::onEnd() {
  m_gameEngine->loadScene("Menu", std::make_shared<MenuScene>(m_gameEngine));
}

void ScoreScene::sRender() {
  SDL_Renderer *renderer = m_gameEngine->getRenderer();
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);
  renderText();
  SDL_RenderPresent(renderer);
}

void ScoreScene::renderText() {
  SDL_Renderer *renderer      = m_gameEngine->getRenderer();
  TTF_Font     *fontMd        = m_gameEngine->getFontMd();
  SDL_Color     gameOverColor = {255, 0, 0, 255};
  SDL_Color     textColor     = {255, 255, 255, 255};

  if (fontMd == nullptr) {
    std::cerr << "Failed to load font" << std::endl;
    return;
  }

  const std::string gameOverText = "Game Over!";
  const Vec2        gameOverPos  = {100, 50};
  TextHelpers::renderLineOfText(renderer, fontMd, gameOverText, gameOverColor, gameOverPos);

  const std::string scoreText = "Score: " + std::to_string(m_score);
  const Vec2        scorePos  = {100, 100};
  TextHelpers::renderLineOfText(renderer, fontMd, scoreText, textColor, scorePos);

  const std::string playAgainText = "Press Enter to play again";
  const Vec2        playAgainPos  = {100, 150};
  TextHelpers::renderLineOfText(renderer, fontMd, playAgainText, textColor, playAgainPos);
}

void ScoreScene::sDoAction(Action &action) {
  if (action.getName() == "SELECT" && action.getState() == ActionState::START) {
    onEnd();
  }
}
