#include "common.hpp"

#include "logger.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <sstream>


void log_lib_version(const SDL_version& compiled, const SDL_version& linked, const std::string& lib) {
    std::stringstream compiled_builder, linked_builder;
    compiled_builder << lib << ": compiled with v" << static_cast<int>(compiled.major)
                    << "." << static_cast<int>(compiled.minor)
                    << "." << static_cast<int>(compiled.patch);

    linked_builder << lib <<": linked with v" << static_cast<int>(linked.major)
                    << "." << static_cast<int>(linked.minor)
                    << "." << static_cast<int>(linked.patch);

    Logger::instance().debug(compiled_builder.str());
    Logger::instance().debug(linked_builder.str());
}


Vec2i camera_project_point(SDL_Window *window, Vec2i point, Vec2i camera_pos) {
    UNUSED(window);

    return point - camera_pos;
    // return vec2f_add(
    //            vec2f_sub(point, camera_pos),
    //            vec2f_mul(window_size(window), vec2fs(0.5f)));
}


SDL_Rect resize_to_char_size(const SDL_Rect& r, int w, int h) {
    return {
        r.x / (w * FONT_SCALE),
        r.y / (h * FONT_SCALE),
        r.w / (w * FONT_SCALE),
        r.h / (h * FONT_SCALE)
    };
}

const std::string selection_shape_as_str(SelectionShape shape) {
    switch (shape) {
        case SelectionShape::NONE:          return "NONE";
        case SelectionShape::TEXT_LIKE:     return "TEXT_LIKE";
        case SelectionShape::RECTANGULAR:   return "RECTANGULAR";
        default:
            Logger::instance().critical(std::string("Unknown selection shape in ") + __func__);
    }
    return "UNHANDLED_SELECTION_SHAPE";
}
