#pragma once
#include <SDL2/SDL.h>
#include <filesystem>
#include <unordered_map>

enum class ImageName { DEFAULT, EXAMPLE };
const std::unordered_map<ImageName, std::filesystem::path> imagePaths = {
    {ImageName::DEFAULT, "assets/images/default.jpg"},
    {ImageName::EXAMPLE, "assets/images/example.jpg"}};
class ImageManager {

public:
  explicit ImageManager();

  static SDL_Surface *loadImage(ImageName name);
  static SDL_Surface *getImage(ImageName name);
};
