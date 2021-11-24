#include "keyboard.hpp"

#include "common.hpp"

#include <SDL2/SDL_keyboard.h>


bool Keyboard::shift_pressed() {
    const Uint8* keyboard = sdlp(SDL_GetKeyboardState(nullptr));
    bool shift_down = static_cast<bool>(keyboard[SDL_SCANCODE_LSHIFT]) || \
                        static_cast<bool>(keyboard[SDL_SCANCODE_RSHIFT]);
    return shift_down;
}


bool Keyboard::ctrl_pressed() {
    const Uint8* keyboard = sdlp(SDL_GetKeyboardState(nullptr));
    bool ctrl_down = static_cast<bool>(keyboard[SDL_SCANCODE_LCTRL]) || \
                        static_cast<bool>(keyboard[SDL_SCANCODE_RCTRL]);
    return ctrl_down;
}

bool Keyboard::alt_pressed() {
    const Uint8* keyboard = sdlp(SDL_GetKeyboardState(nullptr));
    bool alt_down = static_cast<bool>(keyboard[SDL_SCANCODE_LALT]) || \
                        static_cast<bool>(keyboard[SDL_SCANCODE_RALT]);
    return alt_down;
}
