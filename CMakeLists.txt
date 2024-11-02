cmake_minimum_required(VERSION 3.21...3.28)
project(SDL_GAME VERSION 1.0 LANGUAGES CXX)

include(FetchContent)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)


FetchContent_Declare(
  SDL2
  GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
  GIT_TAG release-2.30.8
)
FetchContent_MakeAvailable(SDL2)
if(APPLE)
  target_compile_options(SDL2 PRIVATE
    "-Wno-deprecated-declarations"
    "-Wno-deprecated"
  )

  target_compile_options(SDL2-static PRIVATE
    "-Wno-deprecated-declarations"
    "-Wno-deprecated"
  )

  set_target_properties(SDL2 PROPERTIES
    COMPILE_FLAGS "-Wno-deprecated-declarations -Wno-deprecated"
  )
  set_target_properties(SDL2-static PROPERTIES
    COMPILE_FLAGS "-Wno-deprecated-declarations -Wno-deprecated"
  )
endif()

FetchContent_Declare(
  SDL_ttf
  GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
  GIT_TAG release-2.22.0
)
FetchContent_MakeAvailable(SDL_ttf)

if(EMSCRIPTEN)
  # SDL2 options
  set(SDL_SHARED OFF CACHE BOOL "Build a shared version of the library")
  set(SDL_STATIC ON CACHE BOOL "Build a static version of the library")
  set(SDL_TEST OFF CACHE BOOL "Build the SDL2 test suite")

  set(SDL_DISKAUDIO OFF CACHE BOOL "")
  set(SDL_JACK OFF CACHE BOOL "")
  set(SDL_ALSA OFF CACHE BOOL "")
  set(SDL_PipeWire OFF CACHE BOOL "")
  set(SDL_PulseAudio OFF CACHE BOOL "")
  set(SDL_ESD OFF CACHE BOOL "")
  set(SDL_ARTS OFF CACHE BOOL "")
  set(SDL_NAS OFF CACHE BOOL "")
  set(SDL_SndIO OFF CACHE BOOL "")
  set(SDL_FUSIONSOUND OFF CACHE BOOL "")
  set(SDL_POWER OFF CACHE BOOL "")
  set(SDL_FILESYSTEM OFF CACHE BOOL "")

  # SDL_ttf options
  set(SDL2TTF_SAMPLES OFF CACHE BOOL "")
  set(SDL2TTF_INSTALL OFF CACHE BOOL "")
  set(SDL2TTF_VENDORED ON CACHE BOOL "")
  set(BUILD_SHARED_LIBS OFF CACHE BOOL "")
  set(SDL2TTF_SHARED OFF CACHE BOOL "")
  set(SDL2TTF_STATIC ON CACHE BOOL "")
else()
  set(CMAKE_CXX_CLANG_TIDY "clang-tidy;-checks=modernize-*,performance-*,bugprone-*clang-analyzer-*,")
endif()

set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(OBJ_DIR "${CMAKE_BINARY_DIR}/objects")

file(GLOB_RECURSE SRC_FILES "${SRC_DIR}/*.cpp")

add_executable(${PROJECT_NAME} ${SRC_FILES})

if(EMSCRIPTEN)
  set_target_properties(${PROJECT_NAME} PROPERTIES
    CMAKE_EXECUTABLE_SUFFIX ".js"
  )

  target_link_options(${PROJECT_NAME} PRIVATE
    "-sALLOW_MEMORY_GROWTH=1"
    "-sINITIAL_MEMORY=64MB"
    "-sFULL_ES2=1"
    "-sUSE_WEBGL2=1"
    "-sUSE_SDL_MIXER=2"
    "-sUSE_WEBGPU=1"
    "--preload-file=${CMAKE_SOURCE_DIR}/assets@assets"
  )

  add_custom_target(copy_templates ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/template"
    "${CMAKE_BINARY_DIR}/build"
    COMMENT "Copying template directory to build directory"
  )

  target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2-static SDL2_ttf::SDL2_ttf-static)
else()
  add_custom_target(copy_assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/assets"
    "${CMAKE_BINARY_DIR}/build/assets"
    COMMENT "Copying assets directory to build directory"
  )

  target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2 SDL2_ttf)
endif()


target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)

set_target_properties(${PROJECT_NAME} PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build"
)
