#include "keyboard.hpp"

#include <SDL2/SDL_keyboard.h>


bool Keyboard::shift_pressed() {
    const Uint8* keyboard = SDL_GetKeyboardState(NULL);
    bool shift_down = static_cast<bool>(keyboard[SDL_SCANCODE_LSHIFT]) || \
                        static_cast<bool>(keyboard[SDL_SCANCODE_RSHIFT]);
    return shift_down;
}


bool Keyboard::ctrl_pressed() {
    const Uint8* keyboard = SDL_GetKeyboardState(NULL);
    bool ctrl_down = static_cast<bool>(keyboard[SDL_SCANCODE_LCTRL]) || \
                        static_cast<bool>(keyboard[SDL_SCANCODE_RCTRL]);
    return ctrl_down;
}
