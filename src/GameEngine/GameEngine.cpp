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
    std::cerr << "Assets folder not found." << std::endl;
    return;
  }

  m_configManager = ConfigManager();

  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::cerr << "SDL video system is not ready to go: " << SDL_GetError() << std::endl;
    return;
  }
  std::cout << "SDL video system is ready to go" << std::endl;

  if (TTF_Init() != 0) {
    std::cerr << "TTF_Init: " << TTF_GetError() << std::endl;
    return;
  }

  const std::string &fontPath = m_configManager.getGameConfig().fontPath;

  m_font_sm = TTF_OpenFont(fontPath.c_str(), 14);
  m_font_md = TTF_OpenFont(fontPath.c_str(), 28);
  m_font_lg = TTF_OpenFont(fontPath.c_str(), 48);

  if (!m_font_md || !m_font_sm) {
    std::cerr << "Failed to load fonts: " << TTF_GetError() << std::endl;
    return;
  }
  std::cout << "Fonts loaded successfully!" << std::endl;

  const std::string &WINDOW_TITLE = m_configManager.getGameConfig().windowTitle;
  const Vec2        &WINDOW_SIZE  = m_configManager.getGameConfig().windowSize;

  m_window =
      SDL_CreateWindow(WINDOW_TITLE.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       WINDOW_SIZE.x, WINDOW_SIZE.y, SDL_WINDOW_SHOWN);
  if (m_window == nullptr) {
    std::cerr << "Window could not be created: " << SDL_GetError() << std::endl;
    SDL_Quit();
    return;
  }
  std::cout << "Window created successfully!" << std::endl;

  m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
  if (m_renderer == nullptr) {
    std::cerr << "Renderer could not be created: " << SDL_GetError() << std::endl;
    SDL_DestroyWindow(m_window);
    SDL_Quit();
    return;
  }
  std::cout << "Renderer created successfully!" << std::endl;

  SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
  SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);
  SDL_RenderClear(m_renderer);
  SDL_RenderPresent(m_renderer);

  m_isRunning = true; // Set isRunning to true after successful initialization

  std::cout << "Game initialized successfully!" << std::endl;

  std::shared_ptr<Scene> menuScene = std::make_shared<MenuScene>(this);
  loadScene("Menu", menuScene);
}

GameEngine::~GameEngine() {
  if (m_renderer != nullptr) {
    SDL_DestroyRenderer(m_renderer);
    m_renderer = nullptr;
  }
  if (m_window != nullptr) {
    SDL_DestroyWindow(m_window);
    m_window = nullptr;
  }

  if (m_font_md != nullptr) {
    TTF_CloseFont(m_font_md);
    m_font_md = nullptr;
  }

  if (m_font_sm != nullptr) {
    TTF_CloseFont(m_font_sm);
    m_font_sm = nullptr;
  }
  TTF_Quit();
  SDL_Quit();
  std::cout << "Cleanup completed, SDL exited." << std::endl;
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

bool GameEngine::isRunning() {
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

void GameEngine::loadScene(const std::string &sceneName, std::shared_ptr<Scene> scene) {
  SDL_FlushEvents(SDL_EventType::SDL_KEYDOWN, SDL_EventType::SDL_KEYUP);
  m_scenes[sceneName] = scene;

  scene->setStartTime(SDL_GetTicks64());
  m_currentSceneName = sceneName;
}

ConfigManager &GameEngine::getConfigManager() {
  return m_configManager;
}

void GameEngine::sUserInput() {
  SDL_Event event;

  const std::shared_ptr<Scene> activeScene = m_scenes[m_currentSceneName];

  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT) {
      quit();
    } else if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
      // If current scene doesn't have an action associated with this key, skip it
      if (activeScene->getActionMap().find(event.key.keysym.sym) ==
          activeScene->getActionMap().end()) {
        continue;
      }

      // Determine start or end action by whether key was pressed or released
      const ActionState actionState =
          event.type == SDL_KEYDOWN ? ActionState::START : ActionState::END;

      const std::string &actionName = activeScene->getActionMap().at(event.key.keysym.sym);
      Action             action(actionName, actionState);
      activeScene->sDoAction(action);
      std::cout << "ACTION NAME: " << action.getName() << std::endl;
      std::cout << "ACTION STATE: (0: START, 1: END) " << action.getState() << std::endl;
      std::cout << "ACTION KEY: " << event.key.keysym.sym << std::endl;
      std::cout << "SCENE NAME: " << m_currentSceneName << std::endl;
    }
  }
}

void GameEngine::mainLoop(void *arg) {
  auto *gameEngine = static_cast<GameEngine *>(arg);
  gameEngine->sUserInput();
  gameEngine->update();
}
