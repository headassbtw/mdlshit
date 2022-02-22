# Model converter go brr

## instructions:

These will assume you're on linux, if you aren't, I genuinely don't know what to tell you lmao

### Linux:

1. Run `cmake -S ./ -B ./build`
2. Run `cmake --build ./build`
3. Profit

### Windows:

1. You must have MinGW gcc installed, idk what libs it needs, just shove the entire AUR onto your ssd
2. Make a build directory and cd into it
3. Run `cmake -DCMAKE_TOOLCHAIN_FILE=./../CMake_Modules/Toolchain-cross-mingw32-linux.cmake ..`
4. Run `make`
5. Profit
