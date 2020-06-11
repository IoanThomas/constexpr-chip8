![Build Status](https://github.com/IoanThomas/constexpr-chip8/workflows/build/badge.svg)

# constexpr-chip8

A compile-time CHIP-8 emulator.

The emulator makes extensive use of the C++ keyword `constexpr` meaning that most opcodes can be evaluated at compile-time assuming the CHIP-8 code to be run is available then.
Instructions that cannot be evaluated at compile-time, such as key input, can be executed at runtime, however.

![My emulator running PONG2](images/pong2_screenshot.png)

## Inspiration

This project was inspired by Jason Turner's [ARM emulator](https://github.com/lefticus/cpp_box), which through good C++ practices and heavy use of `constexpr` could be made to execute code at compile-time.
The CppCon talk he did on it can be found [here](https://www.youtube.com/watch?v=DHOlsEd0eDE) which highlights just some of the great things you can do with modern C++.
I was especially interested in the testing part of it - which he notes could also be done at compile-time, meaning that if the tests simply compile then they have already succeeded.

## Getting Started

### Prerequisites

The code requires C++17 or later to compile, and CMake is used for building project files.
In order to support C++17, a CMake version of 3.9 or later is required.

All dependencies required are downloaded using Git submodules during project generation, so other than Git no other programs are required to be installed.

### Building

In the project directory, run the following commands:

```sh
mkdir build
cd build
cmake ..
```

This will generate project files in the `build` folder.

### Testing

Since the entire emulator can run at compile-time, simply compiling the tests successfully should indicate that they pass.
To do this, run the following command from the project directory:

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
