# Console Chess

## About the game
This repository contains a Chess game written in C++ using Intellij's Clion IDE. The game is built using Object Oriented Programming principles.

Every piece can move in a way that is consistent with the rules of Chess. The game also supports castling and en passant. The game is played on a standard 8x8 chess board.

---

## How to build the game

To build the game download the repo, navigate to the root directory and run the following command:
``` Cmake
mkdir build && cd build
cmake ../src && cmake --build . --clean-first --parallel
```
Please note that this project was built using CMake version 3.22.1. If you are using a different version of CMake, you may need to change the CMakeLists.txt file to reflect the version of CMake you are using.


---

## Last words

The game has been tested on Linux and should also work when compiling under Windows, although it has not been thoroughly tested on Windows. If you encounter any bugs or issues, please open an issue on this repository and let us know.

Enjoy the game!