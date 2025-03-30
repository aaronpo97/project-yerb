#pragma once
#include <SDL2/SDL.h>
#include <filesystem>
#include <unordered_map>

enum class SurfaceName { DEFAULT, EXAMPLE };
const std::unordered_map<SurfaceName, std::filesystem::path> imagePaths = {
    {SurfaceName::EXAMPLE, "assets/images/example.png"}};

class SurfaceManager {
  std::unordered_map<SurfaceName, SDL_Surface *> m_surfaces = {};

public:
  explicit SurfaceManager();
  ~SurfaceManager();

  void         loadSurface(SurfaceName name);
  SDL_Surface *getSurface(SurfaceName name);
};
