cmake_minimum_required(VERSION 3.21...3.28)
project(SDL_GAME VERSION 1.0 LANGUAGES CXX)

include(FetchContent)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)


if(NOT EMSCRIPTEN)
        FetchContent_Declare(
                SDL2
                GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
                GIT_TAG release-2.30.8
        )

        FetchContent_MakeAvailable(SDL2)

        FetchContent_Declare(
                SDL_ttf
                GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
                GIT_TAG release-2.22.0
        )

        FetchContent_MakeAvailable(SDL_ttf)

        # Disable all audio formats except OGG and WAV
        set(SDL2MIXER_OPUS OFF CACHE BOOL "")
        set(SDL2MIXER_OPUS_SHARED OFF CACHE BOOL "")
        set(SDL2MIXER_FLAC OFF CACHE BOOL "")
        set(SDL2MIXER_MOD OFF CACHE BOOL "")
        set(SDL2MIXER_MIDI OFF CACHE BOOL "")
        set(SDL2MIXER_WAVPACK OFF CACHE BOOL "")
        set(SDL2MIXER_MP3 OFF CACHE BOOL "")
        set(SDL2MIXER_OGG ON CACHE BOOL "")


        FetchContent_Declare(
                SDL_mixer
                GIT_REPOSITORY https://github.com/libsdl-org/SDL_mixer.git
                GIT_TAG release-2.8.0
        )

        FetchContent_MakeAvailable(SDL_mixer)

        set(CMAKE_CXX_CLANG_TIDY "clang-tidy")

endif()

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


set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(OBJ_DIR "${CMAKE_BINARY_DIR}/objects")

file(GLOB_RECURSE SRC_FILES "${SRC_DIR}/*.cpp")

add_executable(${PROJECT_NAME} ${SRC_FILES})

if(EMSCRIPTEN)
        set_target_properties(${PROJECT_NAME} PROPERTIES
                CMAKE_EXECUTABLE_SUFFIX ".js"
        )

        target_compile_options(${PROJECT_NAME} PRIVATE
                "-sUSE_SDL=2"
                "-sUSE_SDL_IMAGE=2"
                "-sUSE_SDL_TTF=2"
                "-sUSE_SDL_MIXER=2"
        )

        target_link_options(${PROJECT_NAME} PRIVATE
                "-sUSE_SDL=2"
                "-sUSE_SDL_IMAGE=2"
                "-sUSE_SDL_TTF=2"
                "-sUSE_SDL_MIXER=2"
                "-sALLOW_MEMORY_GROWTH=1"
                "--preload-file=${CMAKE_SOURCE_DIR}/assets@assets"
        )

        add_custom_target(copy_templates ALL
                COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_SOURCE_DIR}/template"
                "${CMAKE_BINARY_DIR}/build"
                COMMENT "Copying template directory to build directory"
        )
else()
        target_link_libraries(${PROJECT_NAME} PRIVATE SDL2 SDL2_ttf SDL2_mixer)
        add_custom_target(copy_assets ALL
                COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_SOURCE_DIR}/assets"
                "${CMAKE_BINARY_DIR}/build/assets"
                COMMENT "Copying assets directory to build directory"
        )
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)

set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build"
)
