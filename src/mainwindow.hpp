#ifndef MAINWINDOW_HPP_
#define MAINWINDOW_HPP_

#include "editor.hpp"

#include <string>
#include <vector>
#include <SDL2/SDL.h>


void init_sdl();
void init_libconfig();

class MainWindow {
public:
    explicit MainWindow(int x, int y, int width, int height);
    ~MainWindow();

    bool init();
    void show();

    void open_file(const char* filepath);
    void save_file();

    void set_title(const std::string& title);
    const std::string& title() const { return title_; }

private:
    SDL_Window *win_impl_;
    std::string title_;
    int x_, y_;
    int width_, height_;

    Editor editor_;
};

#endif // MAINWINDOW_HPP_
