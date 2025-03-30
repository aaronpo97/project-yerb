#include "../../includes/AssetManagement/SurfaceManager.hpp"
#include <SDL2/SDL.h>
#include <SDL_image.h>
#include <filesystem>
#include <iostream>
#include <ranges>

SurfaceManager::SurfaceManager() {
  if (IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) == -1) {
    std::cout << "SDL_image could not initialize! SDL_image Error: %s\n" << IMG_GetError();
  }

  for (const auto &name : imagePaths | std::views::keys) {
    loadSurface(name);
  }
  SDL_Log("ImageManager created");
}

SurfaceManager::~SurfaceManager() {
  std::cout << "ImageManager destroyed\n";
  for (auto &[name, image] : m_surfaces) {
    SDL_FreeSurface(image);
    SDL_Log("Surface %d freed", name);
  }
}

void SurfaceManager::loadSurface(const SurfaceName name) {
  const std::filesystem::path &path = imagePaths.at(name);
  SDL_Surface                 *img  = IMG_Load(path.c_str());

  if (img == nullptr) {
    std::cout << "Unable to load image! SDL_image Error:\n" << path.c_str() << IMG_GetError();
  }

  m_surfaces[name] = img;
}

SDL_Surface *SurfaceManager::getSurface(const SurfaceName name) {
  if (std::ranges::find_if(imagePaths, [&](const auto &pair) { return pair.first == name; }) ==
      imagePaths.end()) {
    std::cout << "Image not found in ImageManager\n";
    return nullptr;
  }

  return m_surfaces[name];
}
