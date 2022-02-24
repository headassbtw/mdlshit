cmake_minimum_required(VERSION 3.20)
project(Qt_Ui)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)



find_package(Qt5 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

add_library(Qt_Ui main.cpp main.hpp main_win.cpp main_win.h main_win.ui)
target_link_libraries(Qt_Ui
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        )

