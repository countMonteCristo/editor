#include "cursor.hpp"

#include "logger.hpp"
#include "settings.hpp"

#include <algorithm>


Cursor::Cursor()
    : text_pos_({0, 0}), cached_x_(0), phase_ms_(0)
{
    const auto& cursor_settings = Settings::const_instance().const_cursor();
    const std::string& shape = cursor_settings.shape;
    auto it = str_to_cursor_shape.find(shape);

    if (it == str_to_cursor_shape.end()) {
        Logger::instance().critical("Unknown cursor shape: " + shape);
    }

    shape_ = it->second;

    blinkrate_ms_ = cursor_settings.blinkrate_ms;
}

void Cursor::set_text_offset(Vec2i d, const Text& text, const Vec2i& window_size) {
    UNUSED(window_size);

    int cur_x = text_pos_.x;
    int cur_y = text_pos_.y;

    int new_y = text_pos_.y + d.y;
    text_pos_.y = bounded(0, text.total_lines() - 1, new_y);

    int new_x = text_pos_.x + d.x;
    text_pos_.x = bounded(0,  text.line_width(text_pos_.y), new_x);
    if ( d.x == 0) {
        text_pos_.x = bounded(0, text.line_width(text_pos_.y), cached_x_);
    }

    if ( (cur_x == 0) && (d.x == -1) && (cur_y > 0) )  {
        text_pos_.y--;
        text_pos_.x = text.line_width(text_pos_.y);
    }

    if ( (cur_x == text.line_width(cur_y)) && (d.x == 1) && (cur_y < text.total_lines() - 1) ) {
        text_pos_.y++;
        text_pos_.x = 0;
    }

    if (d.x != 0) {
        cached_x_ = text_pos_.x;
    }

    // If cursor is going to move above the text - set it to the beginning of the first line
    if (new_y < 0) {
        text_pos_.x = text_pos_.y = 0;
        cached_x_ = text_pos_.x;
    }
    // If cursor is going to move below the text - set it to the ending of the last line
    if (new_y >= text.total_lines()) {
        text_pos_.y = text.total_lines()-1;
        text_pos_.x = text.line_width(text_pos_.y);
        cached_x_ = text_pos_.x;
    }

    phase_ms_ = SDL_GetTicks();
}
