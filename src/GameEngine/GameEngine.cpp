#include "../../includes/GameEngine/GameEngine.hpp"
#include "../../includes/GameScenes/MainScene.hpp"
#include "../../includes/GameScenes/MenuScene.hpp"
#include <filesystem>
#include <iostream>

// emscripten

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

GameEngine::GameEngine() {
  if (!std::filesystem::exists("./assets")) {
    SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Assets folder not found!");
    return;
  }

  m_configManager = std::make_unique<ConfigManager>("./assets/config.json");

  const auto &gameConfig = m_configManager->getGameConfig();

  const std::string &FONT_PATH    = gameConfig.fontPath;
  const std::string &WINDOW_TITLE = gameConfig.windowTitle;
  const Vec2        &WINDOW_SIZE  = gameConfig.windowSize;

  m_fontManager = std::make_unique<FontManager>(FONT_PATH);

  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    SDL_LogError(SDL_LOG_CATEGORY_VIDEO, "SDL video system is not ready to go: %s",
                 SDL_GetError());
    return;
  }
  SDL_LogInfo(SDL_LOG_CATEGORY_VIDEO, "SDL video system initialized successfully!");

  m_window =
      SDL_CreateWindow(WINDOW_TITLE.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       WINDOW_SIZE.x, WINDOW_SIZE.y, SDL_WINDOW_SHOWN);
  if (m_window == nullptr) {

    SDL_LogError(SDL_LOG_CATEGORY_VIDEO, "Window could not be created: %s", SDL_GetError());
    SDL_Quit();
    return;
  }
  SDL_LogInfo(SDL_LOG_CATEGORY_VIDEO, "Window created successfully!");

  m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
  if (m_renderer == nullptr) {
    SDL_LogError(SDL_LOG_CATEGORY_VIDEO, "Renderer could not be created: %s", SDL_GetError());
    SDL_DestroyWindow(m_window);
    SDL_Quit();
    return;
  }
  SDL_LogInfo(SDL_LOG_CATEGORY_VIDEO, "Renderer created successfully!");
  const SDL_Color backgroundColor = {.r = 0, .g = 0, .b = 0, .a = 255};
  SDL_SetRenderDrawColor(m_renderer, backgroundColor.r, backgroundColor.g, backgroundColor.b,
                         backgroundColor.a);
  SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);
  SDL_RenderClear(m_renderer);
  SDL_RenderPresent(m_renderer);

  m_isRunning = true; // Set isRunning to true after successful initialization

  SDL_LogInfo(SDL_LOG_CATEGORY_SYSTEM, "Game engine initialized successfully!");
  std::shared_ptr<Scene> menuScene = std::make_shared<MenuScene>(this);
  loadScene("Menu", menuScene);
}

GameEngine::~GameEngine() {
  if (m_renderer != nullptr) {
    SDL_DestroyRenderer(m_renderer);
    m_renderer = nullptr;
    SDL_LogInfo(SDL_LOG_CATEGORY_SYSTEM, "Renderer cleaned up successfully!");
  }
  if (m_window != nullptr) {
    SDL_DestroyWindow(m_window);
    m_window = nullptr;
    SDL_LogInfo(SDL_LOG_CATEGORY_SYSTEM, "Window cleaned up successfully!");
  }

  SDL_Quit();
  SDL_LogInfo(SDL_LOG_CATEGORY_SYSTEM, "Game engine cleaned up successfully!");
}

void GameEngine::update() {
  const std::shared_ptr<Scene> activeScene = m_scenes[m_currentSceneName];
  if (activeScene != nullptr) {
    m_scenes[m_currentSceneName]->update();
  }
}

SDL_Renderer *GameEngine::getRenderer() {
  return m_renderer;
}

SDL_Window *GameEngine::getWindow() {
  return m_window;
}

bool GameEngine::isRunning() const {
  return m_isRunning;
}

void GameEngine::run() {
#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop_arg(mainLoop, this, 0, 1);
#else
  while (m_isRunning) {
    mainLoop(this);
  }
#endif
}

void GameEngine::quit() {
  m_isRunning = false;
}

void GameEngine::loadScene(const std::string &sceneName, const std::shared_ptr<Scene> &scene) {
  m_scenes[sceneName] = scene;

  scene->setStartTime(SDL_GetTicks64());
  m_currentSceneName = sceneName;
}

ConfigManager &GameEngine::getConfigManager() {
  if (!m_configManager) {
    throw std::runtime_error("ConfigManager not initialized");
  }
  return *m_configManager;
}

FontManager &GameEngine::getFontManager() {
  if (!m_fontManager) {
    throw std::runtime_error("FontManager not initialized");
  }
  return *m_fontManager;
}
void GameEngine::sUserInput() {
  SDL_Event event;

  const std::shared_ptr<Scene> activeScene = m_scenes[m_currentSceneName];

  while (!!SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT) {
      quit();
      return;
    }
    if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
      // If current scene doesn't have an action associated with this key, skip it
      if (activeScene->getActionMap().find(event.key.keysym.sym) ==
          activeScene->getActionMap().end()) {
        continue;
      }

      // Determine start or end action by whether key was pressed or released
      const ActionState actionState =
          event.type == SDL_KEYDOWN ? ActionState::START : ActionState::END;

      const std::string &actionName = activeScene->getActionMap().at(event.key.keysym.sym);
      Action             action(actionName, actionState, std::nullopt);
      activeScene->sDoAction(action);
    }

    if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP) {
      if (activeScene->getActionMap().find(event.button.button) ==
          activeScene->getActionMap().end()) {
        continue;
      }

      const ActionState actionState =
          event.type == SDL_MOUSEBUTTONDOWN ? ActionState::START : ActionState::END;

      const std::string &actionName = activeScene->getActionMap().at(event.button.button);

      const Vec2 mousePosition = {static_cast<float>(event.button.x),
                                  static_cast<float>(event.button.y)};

      Action action(actionName, actionState, mousePosition);
      activeScene->sDoAction(action);
    }
  }
}

void GameEngine::mainLoop(void *arg) {
  auto *gameEngine = static_cast<GameEngine *>(arg);
  gameEngine->sUserInput();
  gameEngine->update();
}
