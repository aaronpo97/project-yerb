# Project Yerb

Project Yerb is a custom game/game engine designed with an _Entity-Component-System_ (ECS) architecture using `SDL2` and
`C++`

## Features

- **Cross-platform** - Current CMake configuration supports compilation for both macOS, and for the Web using Emscripten
  and WebAssembly.
- **Modular ECS architecture** - Scalable with independent systems for input handling, rendering, and more.
- **JSON-based configuration** - Uses the [nlohmann/json](https://github.com/nlohmann/json) library for loading game
  configurations and settings.
- **Modern C++** - Code uses C++20 and aims to follow modern C++ practices, enforced by `clang-tidy`.

## Installation

At this stage in development, binaries will not be released and the project must be built from source.

### Building from source

### Emscripten (Recommended)

To build for Emscripten, run the following commands in the current directory.

```bash
mkdir dist-web
cd dist-web
emcmake cmake ..
cmake --build .
```

Note that a web template is found at /template and will be used as a 'shell' for the project.

#### Serving app locally

To serve the app on your machine, I suggest using Emscripten's built-in HTTP server.

Assuming you have already built the app using the instructions above, you can run:

```bash
cd dist-web/build
emrun --no_browser --port 3333 .
```

### macOS

To build for macOS, run the following commands:

```bash
mkdir dist-macos
cd dist-macos
cmake ..
make 2>&1 | tee build.log
```

## Usage

When running the game, you will immediately be brought into the menu scene. Follow the instructions found at the bottom
of the window.

## Contributions

Contributions are welcome. Please submit issues or pull requests to help improve the project.

## License

This project is licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in
compliance with the License.

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "
AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific
language governing permissions and limitations under the License.

### Commons Clause

In addition to the terms of the Apache License, the following **Commons Clause License Condition** applies:

Without limiting other conditions in the License, the grant of rights under the License will **not include**, and the
License **does not grant you**, the right to:

> **Use this software, or any derivatives of it, for commercial purposes without express consent of the original author,
Aaron Po.**

**Commercial Purposes**: For the purposes of this License, "commercial purposes" refers to any activity that involves
selling, licensing, or otherwise monetizing the software or any derivatives of it. This includes, but is not limited to:

- **Selling the Software**: Distributing the software or its derivatives for a fee or any other form of compensation.
- **Using in Business Operations**: Utilizing the software or its derivatives in a manner that is intended to generate
  revenue or support a commercial operation.

For a full overview over the license, please view `./LICENSE.md`.
