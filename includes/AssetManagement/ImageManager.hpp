#pragma once
#include <SDL2/SDL.h>
#include <unordered_map>
enum class ImageName { DEFAULT, EXAMPLE };
class ImageManager {

  std::unordered_map<ImageName, SDL_Surface *> m_images = {{ImageName::DEFAULT, nullptr}};
  void loadImage(const std::filesystem::path &path, const ImageName name);

public:
  explicit ImageManager();
  ~ImageManager();
  SDL_Surface *getImage(ImageName name);
};
