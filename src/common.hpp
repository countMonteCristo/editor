#ifndef COMMON_HPP_
#define COMMON_HPP_

#include "la.hpp"

#include "logger.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <string>
#include <vector>

#define UNUSED(x) (void)(x)

constexpr int FONT_SCALE = 1;

#define FONT_RENDER_SOLID 0
#define FONT_RENDER_SHADED 1
#define FONT_RENDER_BLENDED 2

#ifndef FONT_RENDER_STYLE
    #define FONT_RENDER_STYLE FONT_RENDER_SOLID
#endif // FONT_RENDER_STYLE


constexpr int FPS = 30;
static const Uint32 FRAME_TIME_MS = static_cast<Uint32>(1000.0 / FPS);

#define UNWRAP_U64(x) \
    static_cast<uint8_t>((x & 0xFF000000) >> 8*3), \
    static_cast<uint8_t>((x & 0x00FF0000) >> 8*2), \
    static_cast<uint8_t>((x & 0x0000FF00) >> 8*1), \
    static_cast<uint8_t>((x & 0x000000FF) >> 8*0)


Vec2i camera_project_point(SDL_Window *window, Vec2i point, Vec2i camera_pos);
SDL_Rect resize_to_char_size(const SDL_Rect& r, int w, int h);
void log_lib_version(const SDL_version& compiled, const SDL_version& linked, const std::string& lib);

template<typename T>
T* sdlp(T* ptr) {
    if ( !ptr ) {
        Logger::instance().critical(std::string("SDL critical error: ") + SDL_GetError());
    }
    return ptr;
}

template<typename T>
T sdli(T val) {
    if ( val ) {
        Logger::instance().error(std::string("SDL error: ") + SDL_GetError());
    }
    return val;
}

template<typename T>
T* ttfp(T* ptr) {
    if ( !ptr ) {
        Logger::instance().critical(std::string("SDL_TTF critical error: ") + TTF_GetError());
    }
    return ptr;
}

template<typename T>
T ttfi(T val) {
    if ( val ) {
        Logger::instance().error(std::string("SDL_TTF error: ") + TTF_GetError());
    }
    return val;
}

#endif // COMMON_HPP_
