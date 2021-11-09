#ifndef CURSOR_HPP_
#define CURSOR_HPP_

#include "la.hpp"
#include "common.hpp"
#include "text.hpp"

#include <SDL2/SDL.h>

#include <unordered_map>

#define CURSOR_BLINK_PERIOD_MSEC 1000

enum class CursorShape {
    IBeam = 0,
    Rect,
    Underscore,
};

const std::unordered_map<std::string, CursorShape> str_to_cursor_shape = {
    {"ibeam",       CursorShape::IBeam},
    {"rect",        CursorShape::Rect},
    {"underscore",  CursorShape::Underscore},
};


class Cursor {
public:
    explicit Cursor();// : text_pos_({0, 0}), cached_x_(0), phase_ms_(0), shape_(CursorShape::Underscore) {}
    ~Cursor() {}

    inline int row() const { return text_pos_.y; }
    inline int col() const { return text_pos_.x; }
    const Vec2i& text_pos() const { return text_pos_; }

    Uint32 phase_ms() const { return phase_ms_; }
    CursorShape shape() const { return shape_; }
    int blinkrate_ms() const { return blinkrate_ms_; }

    void set_text_offset(Vec2i d, const Text& text, const Vec2i& window_size);

private:
    Vec2i text_pos_;
    int cached_x_;

    Uint32 phase_ms_;
    int blinkrate_ms_;
    CursorShape shape_;
};

#endif // CURSOR_HPP_
