#include "../includes/Game.h"

Game::Game() {}

Game::~Game() {
  sCleanup();
}

void Game::init() {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::cerr << "SDL video system is not ready to go: " << SDL_GetError() << std::endl;
    return;
  }
  std::cout << "SDL video system is ready to go" << std::endl;

  window = SDL_CreateWindow("C++ SDL2 Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                            640, 480, SDL_WINDOW_SHOWN);
  if (window == nullptr) {
    std::cerr << "Window could not be created: " << SDL_GetError() << std::endl;
    SDL_Quit();
    return;
  }
  std::cout << "Window created successfully!" << std::endl;

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (renderer == nullptr) {
    std::cerr << "Renderer could not be created: " << SDL_GetError() << std::endl;
    SDL_DestroyWindow(window);
    SDL_Quit();
    return;
  }
  std::cout << "Renderer created successfully" << std::endl;

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);
  SDL_RenderPresent(renderer);

  isRunning = true; // Set isRunning to true after successful initialization
}

void Game::run() {
  while (isRunning) {
    sInput();
    sRender();
  }
  sCleanup();
}

void Game::sInput() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    std::cout << "Event type: " << event.type << std::endl;
    if (event.type == SDL_QUIT) {
      std::cout << "Quit event detected - exiting game loop" << std::endl;
      isRunning = false; // Stop the game loop
      return;
    }
  }
}

void Game::sRender() {
  // Clear the screen
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Set background to black
  SDL_RenderClear(renderer);

  // Draw a red rectangle
  SDL_Rect rect = {10, 10, 100, 100};
  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
  SDL_RenderFillRect(renderer, &rect);

  // Update the screen
  SDL_RenderPresent(renderer);
}

void Game::sCleanup() {
  if (renderer != nullptr) {
    SDL_DestroyRenderer(renderer);
    renderer = nullptr;
  }
  if (window != nullptr) {
    SDL_DestroyWindow(window);
    window = nullptr;
  }
  SDL_Quit();
  std::cout << "Cleanup completed, SDL exited." << std::endl;
}
