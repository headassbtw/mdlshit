#!/bin/bash
mkdir build-windows
cd build-windows
cmake -DCMAKE_TOOLCHAIN_FILE=./../CMake_Modules/Toolchain-cross-mingw32-linux.cmake ..
make -j$(nproc)
cd ..
