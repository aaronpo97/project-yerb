#include "../../includes/AssetManagement/ImageManager.hpp"
#include <SDL2/SDL.h>
#include <SDL_image.h>
#include <filesystem>

void ImageManager::loadImage(const std::filesystem::path &path, const ImageName name) {
  SDL_Surface *img = IMG_Load(path.c_str());
  m_images[name]   = img;
}

ImageManager::ImageManager() {
  if (IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) == -1) {
    printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
    // Handle error
  }

  SDL_Log("ImageManager created");
  loadImage("assets/images/stuff.jpg", ImageName::EXAMPLE);
}
ImageManager::~ImageManager() {
  for (auto &[name, image] : m_images) {
    if (image == nullptr) {
      return;
    }
    SDL_FreeSurface(image);
    image = nullptr;
  }
}
SDL_Surface *ImageManager::getImage(const ImageName name) {
  if (!m_images.contains(name)) {
    return nullptr;
  }
  return m_images[name];
}
