cmake_minimum_required(VERSION 3.22)
project(mdlshit)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/CMake_Modules)

include_directories(include)
file(GLOB_RECURSE SRC CONFIGURE_DEPENDS
    src/*.cpp
)
include(generate_product_version)
generate_product_version(
   VersionFilesOutputVariable
   NAME "Titanfall 2 .MDL converter"

   COMMENTS "Fuck you Respawn!"
   ORIGINAL_FILENAME "mdlshit.exe"
   COMPANY_COPYRIGHT "MasterLiberty & headassbtw (C) Copyright 2087"
   INTERNAL_NAME "fucking bitch ass model converter"
   VERSION_MAJOR 4
   VERSION_MINOR 20
   VERSION_PATCH 69
)
#add_subdirectory(ui)
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden -Wall")
IF(MINGW)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/Windows)
ELSE(MINGW)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/Linux)
ENDIF(MINGW)
add_executable(mdlshit ${SRC} ${VersionFilesOutputVariable})
#set(CMAKE_EXE_LINKER_FLAGS " -static")

target_link_libraries(mdlshit -static-libgcc -static-libstdc++)
