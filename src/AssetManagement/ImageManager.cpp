#include "../../includes/AssetManagement/ImageManager.hpp"
#include <SDL2/SDL.h>
#include <SDL_image.h>
#include <filesystem>
#include <iostream>

SDL_Surface *ImageManager::loadImage(const ImageName name) {
  const std::filesystem::path &path = imagePaths.at(name);
  SDL_Surface                 *img  = IMG_Load(path.c_str());

  if (img == nullptr) {
    std::cout << "Unable to load image! SDL_image Error:\n" << path.c_str() << IMG_GetError();
    return nullptr;
  }

  return img;
}

ImageManager::ImageManager() {
  if (IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) == -1) {
    std::cout << "SDL_image could not initialize! SDL_image Error: %s\n" << IMG_GetError();
  }

  SDL_Log("ImageManager created");
}

SDL_Surface *ImageManager::getImage(const ImageName name) {
  return loadImage(name);
}
