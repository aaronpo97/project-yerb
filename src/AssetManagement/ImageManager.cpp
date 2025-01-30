#include "../../includes/AssetManagement/ImageManager.hpp"
#include <SDL2/SDL.h>
#include <SDL_image.h>
#include <filesystem>
#include <iostream>
#include <ranges>

void ImageManager::loadImage(const ImageName name) {
  const std::filesystem::path &path = imagePaths.at(name);
  SDL_Surface                 *img  = IMG_Load(path.c_str());

  if (img == nullptr) {
    std::cout << "Unable to load image! SDL_image Error:\n" << path.c_str() << IMG_GetError();
  }

  m_images[name] = img;
}

ImageManager::ImageManager() {
  if (IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) == -1) {
    std::cout << "SDL_image could not initialize! SDL_image Error: %s\n" << IMG_GetError();
  }

  for (const auto &name : imagePaths | std::views::keys) {
    loadImage(name);
  }
  SDL_Log("ImageManager created");
}

ImageManager::~ImageManager() {
  // @TODO handle memory management
  // std::cout << "ImageManager destroyed" << std::endl;
  // for (auto &[name, image] : m_images) {
  //   SDL_FreeSurface(image);
  //   SDL_Log("%s surface freed", name);
  // }
}

SDL_Surface *ImageManager::getImage(const ImageName name) {
  if (std::ranges::find_if(imagePaths, [&](const auto &pair) { return pair.first == name; }) ==
      imagePaths.end()) {
    std::cout << "Image not found in ImageManager\n";
    return nullptr;
  }

  return m_images[name];
}
