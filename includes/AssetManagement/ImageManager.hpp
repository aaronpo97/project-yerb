#pragma once
#include <SDL2/SDL.h>
#include <filesystem>
#include <unordered_map>

enum class ImageName { DEFAULT, EXAMPLE };
const std::unordered_map<ImageName, std::filesystem::path> imagePaths = {
    {ImageName::EXAMPLE, "assets/images/example.jpg"}};

class ImageManager {
  const std::unordered_map<ImageName, SDL_Surface *> m_images = {};

public:
  explicit ImageManager();
  std::unordered_map<ImageName, SDL_Surface *> m_images;
  ~ImageManager();

  SDL_Surface *loadImage(ImageName name);
  SDL_Surface *getImage(ImageName name);
};
