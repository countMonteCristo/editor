#include "mainwindow.hpp"

#include "settings.hpp"

#include <SDL2/SDL.h>

#include <iostream>


const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;


int main(int argc, const char* argv[]) {
    Settings::instance().init("editor.conf");
    Logger::instance().init();

    init_sdl();
    init_libconfig();

    MainWindow window(SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT);

    if (!window.init()) {
        Logger::instance().critical("Cannot initialize main window");
    }

    const char* filepath = nullptr;
    if (argc == 2)
        filepath = argv[1];

    window.open_file(filepath);
    window.show();
}
