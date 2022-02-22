#!/bin/bash
mkdir winbuild
cd winbuild
cmake -DCMAKE_TOOLCHAIN_FILE=./../CMake_Modules/Toolchain-cross-mingw32-linux.cmake ..
make
cd ..