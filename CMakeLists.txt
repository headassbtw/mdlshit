cmake_minimum_required(VERSION 3.22)
project(mdlshit)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/CMake_Modules)

include_directories(include)
include_directories(external/imgui)
include_directories(external/glfw/include)
include_directories(external/glew/include)
include_directories(external/tinyfiledialogs)
include_directories(external/glm)
find_package(OpenGL REQUIRED)
file(GLOB_RECURSE SRC CONFIGURE_DEPENDS
    src/*.cpp
    external/tinyfiledialogs/*.c
)
file(GLOB IM_SRC CONFIGURE_DEPENDS
    "external/imgui/*.cpp"
    "external/imgui/misc/*.cpp"
    "external/imgui/backends/imgui_impl_opengl3.cpp"
    "external/imgui/backends/imgui_impl_glfw.cpp"
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
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_BUILD_TESTS OFF)
set(BUILD_UTILS OFF)

add_library(ImGui STATIC ${IM_SRC})
add_subdirectory(external/glfw)
add_subdirectory(external/glew/build/cmake)

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden -Wall")
IF(WIN32 OR MINGW)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/Windows)
    set(CMAKE_EXE_LINKER_FLAGS " -static")
ELSE(WIN32 OR MINGW)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/Linux)
ENDIF(WIN32 OR MINGW)

IF(WIN32 OR MINGW)
    add_executable(mdlshit WIN32 ${SRC} ${VersionFilesOutputVariable})
ELSE(WIN32 OR MINGW)
    add_executable(mdlshit ${SRC} ${VersionFilesOutputVariable})
ENDIF(WIN32 OR MINGW)

IF(APPLE)
target_link_libraries(mdlshit glew_s glfw ImGui -s)
ELSE(APPLE)
target_link_libraries(mdlshit glew_s glfw ImGui -static-libgcc -static-libstdc++ -s)
ENDIF(APPLE)