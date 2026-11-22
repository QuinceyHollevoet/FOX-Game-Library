cmake_minimum_required(VERSION 3.16)
project(FoxGameLibrary VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Concurrent Svg Network)

qt_add_executable(FoxGameLibrary
    "main.cpp"
    "mainWindow.h"
    "mainWindow.cpp"
    "assets.qrc"
    "toggleTheme.h"
    "gameScanner.h"
    "messageBox.h"
)

set_target_properties(FoxGameLibrary PROPERTIES
    OUTPUT_NAME "FOX Game Library"
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

target_link_libraries(FoxGameLibrary PRIVATE 
    Qt6::Core 
    Qt6::Widgets 
    Qt6::Concurrent
    Qt6::Svg
)

qt_add_executable(Uninstaller
    "uninstaller_main.cpp"
)

set_target_properties(Uninstaller PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE FALSE
)

target_link_libraries(Uninstaller PRIVATE 
    Qt6::Core 
    Qt6::Widgets
)

qt_add_executable(Updater
    "updater_main.cpp"
)

set_target_properties(Updater PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE FALSE
)

target_link_libraries(Updater PRIVATE 
    Qt6::Core 
    Qt6::Widgets
    Qt6::Network
)
