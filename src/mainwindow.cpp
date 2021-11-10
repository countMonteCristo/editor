#include "mainwindow.hpp"

#include "common.hpp"
#include "keyboard.hpp"
#include "settings.hpp"
#include "selection.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <sstream>
#include <iostream>


void init_sdl(void) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        Logger::instance().critical(std::string("SDL_Init Error: ") + SDL_GetError());
    } else {
        Logger::instance().debug("SDL2: successfully initialized");
    }

    SDL_version compiled;
    SDL_version linked;

    SDL_VERSION(&compiled);
    SDL_GetVersion(&linked);

    log_lib_version(compiled, linked, "SDL2");
}

void init_libconfig() {
    std::stringstream compiled_builder;
    compiled_builder << "Libconfig: using v" << LIBCONFIGXX_VER_MAJOR
                    << "." << LIBCONFIGXX_VER_MINOR
                    << "." << LIBCONFIGXX_VER_REVISION;

    Logger::instance().debug(compiled_builder.str());
}


MainWindow::MainWindow(int x, int y, int width, int height)
    : win_impl_(nullptr), x_(x), y_(y), width_(width), height_(height)
{
    editor_.handle_window_size_changed(width_ ,height_);
}

MainWindow::~MainWindow() {
    if ( win_impl_ )
        SDL_DestroyWindow(win_impl_);
}

bool MainWindow::init() {
    win_impl_ = SDL_CreateWindow(nullptr, x_, y_, width_, height_, SDL_WINDOW_RESIZABLE);

    if (win_impl_ == NULL) {
        return false;
    }

    return true;
}

void MainWindow::show() {

    SDL_Event event;
    bool quit = false;

    SDL_Renderer *renderer = SDL_CreateRenderer(win_impl_, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        Logger::instance().critical(std::string("Can't create renderer: ") + SDL_GetError());
    }

    editor_.set_renderer(renderer, win_impl_);

    #ifdef DEBUG
    int nk = 0;
    #endif

    while (!quit) {
        const Uint32 start = SDL_GetTicks();

        // TODO: separate actions (select all, save, copy, paste, etc.) from key bindings
        // TODO: scrollbars
        // TODO: select word on double-click
        // TODO: select line on triple-click
        // TODO: multiple cursors
        // TODO: command line for searching, replacing, etc
        // TODO: Ctrl-X for cut selected text
        // TODO: menu bar
        // TODO: chamge mouse cursor to bar when it is above the text area
        // TODO: settings for customizing text cursor (bar, rect, something else)

        while(SDL_PollEvent(&event) != 0) {
            bool control_down = Keyboard::ctrl_pressed();

            switch (event.type) {
                case SDL_QUIT: {
                    quit = true;
                    break;
                }
                case SDL_MOUSEBUTTONDOWN: {
                    editor_.handle_mouse_click(event.button);
                    break;
                }
                case SDL_MOUSEBUTTONUP: {
                    editor_.handle_mouse_button_up(event.button);
                    break;
                }
                case SDL_MOUSEMOTION: {
                    editor_.handle_mouse_move(event.motion);
                    break;
                }
                case SDL_MOUSEWHEEL: {
                    editor_.handle_wheel(event.wheel);
                    break;
                }
                case SDL_TEXTINPUT: {
                    editor_.handle_text_input(event.text.text);
                    break;
                }
                case SDL_KEYDOWN: {
                    switch (event.key.keysym.sym) {
                        case SDLK_c: {
                            if (control_down)
                                editor_.selection_to_clipboard();
                            break;
                        }
                        case SDLK_v: {
                            if (control_down)
                                editor_.insert_from_clipboard();
                            break;
                        }
                        case SDLK_a: {
                            if (control_down)
                                editor_.select_all();
                            break;
                        }
                        case SDLK_s: {
                            if (control_down)
                                save_file();
                            break;
                        }
                        case SDLK_d: {
                            if (control_down)
                                editor_.log_debug_history();
                            break;
                        }
                        case SDLK_z: {
                            if (control_down)
                                editor_.handle_undo();
                            break;
                        }
                        case SDLK_y: {
                            if (control_down)
                                editor_.handle_redo();
                            break;
                        }
                        case SDLK_RIGHT: case SDLK_LEFT: case SDLK_UP: case SDLK_DOWN: {
                            editor_.handle_keyboard_move_pressed();
                            break;
                        }
                        case SDLK_RETURN: {
                            editor_.handle_return_pressed();
                            break;
                        }
                        case SDLK_BACKSPACE: {
                            editor_.handle_backspace();
                            break;
                        }
                        case SDLK_DELETE: {
                            editor_.handle_delete();
                            break;
                        }
                        case SDLK_HOME: {
                            editor_.handle_home_pressed();
                            break;
                        }
                        case SDLK_END: {
                            editor_.handle_end_pressed();
                            break;
                        }
                        case SDLK_PAGEUP: {
                            editor_.handle_pageup_pressed();
                            break;
                        }
                        case SDLK_PAGEDOWN: {
                            editor_.handle_pagedown_pressed();
                            break;
                        }
                    }
                    break;
                }
                case SDL_KEYUP: {
                    switch (event.key.keysym.sym) {
                        case SDLK_LSHIFT: case SDLK_RSHIFT: {
                            editor_.handle_shift_released();
                            break;
                        }
                    }
                    break;
                }
                case SDL_WINDOWEVENT: {
                    switch (event.window.event) {
                        case SDL_WINDOWEVENT_SIZE_CHANGED: {
                            editor_.handle_window_size_changed(event.window.data1, event.window.data2);
                            break;
                        }
                    }
                    break;
                }
            }
        }
        editor_.render();

        const Uint32 delta_ms = SDL_GetTicks() - start;

        #ifdef DEBUG
        if (nk == FPS / 2) {
            float fps = 1000.0f / static_cast<float>(delta_ms);
            std::stringstream ss;
            ss.precision(2);
            ss << std::fixed << fps;
            set_title("Editor | FPS=" + ss.str());
        }
        nk = (nk + 1) % FPS;
        #endif

        if (delta_ms < FRAME_TIME_MS)
            SDL_Delay(FRAME_TIME_MS - delta_ms);
    }

    SDL_Quit();
}


void MainWindow::open_file(const char* filepath) {
    std::string title = "Editor";
    if ( filepath ) {
        editor_.load_from_file(filepath);
        title += " | " + editor_.filepath();
    }
    set_title(title);
}


void MainWindow::save_file() {
    std::string title = std::string("Editor | ") + editor_.filepath();
    editor_.save_to_file();
    set_title(title);
}


void MainWindow::set_title(const std::string& title) {
    title_ = title;
    SDL_SetWindowTitle(win_impl_, title_.c_str());
}
