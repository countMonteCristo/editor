cmake_minimum_required(VERSION 3.10)

set(CMAKE_VERBOSE_MAKEFILE OFF)

project(main)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -std=c++11")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_definitions(-DFONT_RENDER_STYLE=FONT_RENDER_BLENDED)

INCLUDE(FindPkgConfig)

PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
PKG_SEARCH_MODULE(SDL2_ttf REQUIRED)
PKG_SEARCH_MODULE(LIBCONFIG++ REQUIRED)
PKG_SEARCH_MODULE(FONTCONFIG REQUIRED fontconfig)

include_directories(${SDL2_ttf_INCLUDE_DIRS} ${LIBCONFIG++_INCLUDE_DIRS} ${FONTCONFIG_INCLUDE_DIRS})


set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/editor.cpp
    src/cursor.cpp
    src/renderer.cpp
    src/common.cpp
    src/font.cpp
    src/selection.cpp
    src/glyph.cpp
    src/history.cpp
    src/document.cpp
    src/settings.cpp
    src/keyboard.cpp
    src/logger.cpp
    src/text.cpp
)

add_executable(main ${SOURCES})
target_link_libraries(main ${SDL2_ttf_LIBRARIES} ${LIBCONFIG++_LIBRARIES} ${FONTCONFIG_LIBRARIES})
