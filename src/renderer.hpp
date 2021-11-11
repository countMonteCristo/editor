#ifndef RENDERER_HPP_
#define RENDERER_HPP_

#include "la.hpp"
#include "font.hpp"
#include "text.hpp"
#include "common.hpp"
#include "cursor.hpp"
#include "selection.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <unordered_map>


class EditorRenderer {
public:
    explicit EditorRenderer();
    ~EditorRenderer();

    void render_glyph(const Glyph& glyph, const SDL_Rect& dst_rect);
    void render_text_line(const line_t& line, const size_t count, Vec2i pos);

    void render_text(const Text& text, Vec2i pos, Vec2i camera_pos);
    void render_cursor(const Cursor& cursor, const Text& text, Vec2i camera_pos);
    void render_rulers(Vec2i camera_pos);
    void render_line_numbers(const Text& text, Vec2i camera_pos);
    void render_info_panel(const Cursor& cursor);

    void render_selection(const Selection& selection, const Text& text, Vec2i camera_pos);

    void render_editor_area(const Cursor& cursor, const Text& lines, const Selection& selection, Vec2i camera_pos);

    const SDL_Rect& line_no_viewport() const { return line_no_viewport_; }
    const SDL_Rect& text_viewport() const { return text_viewport_; }

    void set_renderer( SDL_Renderer* r) { renderer_impl_ = r; font_.set_renderer(r); }
    void set_window( SDL_Window* w) { window_ = w; }

    void set_editor_window_size(int width, int heigth);
    void adjust_viewports(int line_no_chars_width);
    void set_line_no_viewport(const SDL_Rect& r) {line_no_viewport_ = r;}
    void set_text_viewport(const SDL_Rect& r) {text_viewport_ = r;}

    inline int font_width() const { return font_.width(); }
    inline int font_height() const { return font_.height(); }

private:

private:
    SDL_Renderer* renderer_impl_;
    SDL_Window* window_;

    SDL_Rect line_no_viewport_;
    SDL_Rect text_viewport_;
    SDL_Rect info_viewport_;

    SDL_Rect window_rect_;

    Font font_;
};


#endif // RENDERER_HPP_
