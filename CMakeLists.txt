cmake_minimum_required(VERSION 3.22)
project(mdlshit)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/CMake_Modules)

find_package(OpenGL REQUIRED)

include_directories(include)
include_directories(external/imgui)
include_directories(external/glfw/include)
include_directories(external/glfw/src)
include_directories(external/glew/include)
include_directories(external/tinyfiledialogs)
include_directories(external/glm)
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


set(APP_COPYRIGHT "MasterLiberty & headassbtw (C) Copyright 2022") #2678 for shitpost
set(APP_NAME "Titanfall 2 .MDL converter")
set(APP_INFOSTRING "Fuck you Respawn!")
generate_product_version(
   VersionFilesOutputVariable
   NAME ${APP_NAME}

   COMMENTS ${APP_INFOSTRING}
   ORIGINAL_FILENAME "mdlshit.exe"
   COMPANY_COPYRIGHT ${APP_COPYRIGHT}
   INTERNAL_NAME "fucking bitch ass model converter"
   VERSION_MAJOR 1
   VERSION_MINOR 0
   VERSION_PATCH 0
)
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_BUILD_TESTS OFF)
set(BUILD_UTILS OFF)

add_library(ImGui STATIC ${IM_SRC})
set(GLFW_LIBRARY_TYPE STATIC)
add_subdirectory(external/glfw)
add_subdirectory(external/glew/build/cmake)

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden -Wall")
if(WIN32)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/Windows)
    set(CMAKE_EXE_LINKER_FLAGS " -static")
elseif(APPLE)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/Mac)
else()
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/Linux)
endif()

if(WIN32)
    add_executable(mdlshit WIN32 ${SRC} ${VersionFilesOutputVariable})
elseif(APPLE)
    
    set(myApp_ICON ${CMAKE_CURRENT_SOURCE_DIR}/product.icns)
    set_source_files_properties(${myApp_ICON} PROPERTIES
       MACOSX_PACKAGE_LOCATION "Resources")
    set(MACOSX_BUNDLE_ICON_FILE product.icns)
    set(MACOSX_BUNDLE_BUNDLE_NAME ${APP_NAME})
    set(MACOSX_BUNDLE_COPYRIGHT ${APP_COPYRIGHT})
    set(MACOSX_BUNDLE_INFO_STRING ${APP_INFOSTRING})
    set(MACOSX_BUNDLE_BUNDLE_VERSION "4.20.69")
    add_executable(mdlshit MACOSX_BUNDLE ${SRC} ${myApp_ICON})
else()
    add_executable(mdlshit ${SRC})
endif()


IF(APPLE)
target_link_libraries(mdlshit glew_s glfw ImGui "-framework Cocoa"
                                       "-framework IOKit"
                                       "-framework CoreFoundation")
ELSE(APPLE)
target_link_libraries(mdlshit ImGui glew_s glfw -static-libgcc -static-libstdc++ -s)
ENDIF(APPLE)