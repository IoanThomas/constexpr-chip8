![Build Status](https://github.com/IoanThomas/constexpr-chip8/workflows/build/badge.svg)

# constexpr-chip8

A compile-time CHIP-8 emulator.

![My emulator running PONG2](images/pong2_screenshot.png)

## Getting Started

### Prerequisites

The code requires C++17 or later to compile, and CMake is used for building project files. In order to support C++17, a CMake version of 3.9 or later is required.

All dependencies required are downloaded using Git submodules during project generation, so other than Git no other programs are required to be installed.

### Building

In the project directory, run the following commands:

```
mkdir build && cd build
cmake ..
```

This will generate project files in the `build` folder.

### Testing

Since the entire emulator can run at compile-time, simply compiling the tests successfully should indicate that they pass. To do this, run the following command from the project directory:

```
cmake --build build --target tests
```

The executable file will be generated in the `build` folder if you want to run the tests directly.

## Tools and Libraries

* [CMake](https://cmake.org/) - Cross-platform build system
* [SFML](https://www.sfml-dev.org/) - Simple cross-platform multimedia library
* [Catch2](https://github.com/catchorg/Catch2) - Test framework for C++

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
