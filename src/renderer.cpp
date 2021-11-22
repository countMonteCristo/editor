#include "renderer.hpp"

#include "logger.hpp"
#include "settings.hpp"

#include <sstream>
#include <iostream>


EditorRenderer::EditorRenderer() {
}

EditorRenderer::~EditorRenderer() {
    if (renderer_impl_)
        SDL_DestroyRenderer(renderer_impl_);
}


void EditorRenderer::render_glyph(const Glyph& glyph, const SDL_Rect& dst_rect) {
    // int w, h;
    SDL_Texture *glyph_texture = font_.get_glyph_texture(glyph);
    // sdli(SDL_QueryTexture(glyph_texture, NULL, NULL, &w, &h));
    sdli(SDL_RenderCopy(renderer_impl_, glyph_texture, NULL, &dst_rect));
}

void EditorRenderer::render_text_line(const line_t& line, const size_t count, Vec2i pos) {
    SDL_Rect dst_rect = {
        0,
        pos.y * font_height() * FONT_SCALE,
        font_width() * FONT_SCALE,
        font_height() * FONT_SCALE,
    };

    size_t i = 0;
    while ( i < std::min(count, line.size())) {
        const Glyph& glyph = line[i];

        // TODO: wrap lines
        // if (pos == window_size_.x) {
        //     pos = 0;
        //     n_line++;
        // }
        dst_rect.x = pos.x * font_width() * FONT_SCALE;

        render_glyph(glyph, dst_rect);
        pos.x++;
        i++;
    }
}

void EditorRenderer::render_text(const Text& text, Vec2i pos, Vec2i camera_pos) {
    const uint32_t color = Settings::const_instance().const_colors().text;
    sdli(SDL_SetRenderDrawColor(renderer_impl_, UNWRAP_U64(color)));

    const SDL_Rect& text_rect = resize_to_char_size(text_viewport_, font_width(), font_height());

    int start = camera_pos.y;
    int stop = std::min(camera_pos.y + text_rect.h + 1, text.total_lines());

    Vec2i pen = camera_project_point(nullptr, pos + Vec2i(0, start), camera_pos);
    int pen_start_x = pen.x;

    for (int i=start; i<stop; i++) {
        const auto& line = text.line_at({0, i});
        pen.x = pen_start_x;
        // TODO: render only text_rect.w glyphs starting from camera_pos.x
        render_text_line(line, line.size(), pen);
        pen.y++;
    }
}

void EditorRenderer::render_cursor(const Cursor& cursor, const Text& text, Vec2i camera_pos) {

    Uint32 time_ms = SDL_GetTicks() - cursor.phase_ms();
    const Vec2i& pos = cursor.text_pos();

    if ( std::sin(2 * M_PI * time_ms / cursor.blinkrate_ms()) > 0) {
        Vec2i pen = camera_project_point(nullptr, pos, camera_pos);
        const uint32_t color = Settings::const_instance().const_colors().cursor;
        sdli(SDL_SetRenderDrawColor(renderer_impl_, UNWRAP_U64(color)));

        SDL_Rect cursor_rect = {
            pen.x * font_width() * FONT_SCALE,
            pen.y * font_height() * FONT_SCALE,
            font_width() * FONT_SCALE,
            font_height() * FONT_SCALE
        };

        const line_t& line = text.line_at(pos);
        const uint32_t inv_color = 0xFFFFFFFF - color;
        Glyph g;

        switch (cursor.shape()) {
        case CursorShape::IBeam:
            sdli(SDL_RenderDrawLine(renderer_impl_, cursor_rect.x, cursor_rect.y,
                                    cursor_rect.x, cursor_rect.y + cursor_rect.h));
            break;
        case CursorShape::Rect:
            sdli(SDL_RenderDrawRect(renderer_impl_, &cursor_rect));
            break;
        case CursorShape::Underscore:
            sdli(SDL_RenderDrawLine(renderer_impl_, cursor_rect.x, cursor_rect.y + cursor_rect.h,
                                    cursor_rect.x + cursor_rect.w, cursor_rect.y + cursor_rect.h));
            break;
        case CursorShape::FilledRect:
            sdli(SDL_RenderFillRect(renderer_impl_, &cursor_rect));
            if ((pos.x >= 0) && (pos.x < text.line_width(pos.y))) {
                g = line[pos.x];
                g.set_color(inv_color);
                render_glyph(g, cursor_rect);
            }
            break;
        default:
            Logger::instance().critical("Cannot draw cursor of unknown shape");
        }

    }
}

void EditorRenderer::render_rulers(Vec2i camera_pos) {
    const std::vector<int>& rulers = Settings::const_instance().const_rulers();

    const uint32_t color = Settings::const_instance().const_colors().ui;
    sdli(SDL_SetRenderDrawColor(renderer_impl_, UNWRAP_U64(color)));

    for (const int max_chars: rulers) {
        Vec2i pen = {(max_chars - camera_pos.x)*font_width(), 0};
        sdli(SDL_RenderDrawLine(renderer_impl_, pen.x, pen.y, pen.x, pen.y + text_viewport_.h));
    }
}

void EditorRenderer::render_line_numbers(const Text& text, Vec2i camera_pos) {
    const SDL_Rect& line_no_rect = resize_to_char_size(line_no_viewport_, font_width(), font_height());

    Vec2i pen = camera_project_point(nullptr, {camera_pos.x, 0}, camera_pos);
    int pen_start_x = pen.x;

    const uint32_t line_no_color = Settings::const_instance().const_colors().line_no;

    for (int line_no = 1; line_no <= text.total_lines(); line_no++) {
        std::string line_no_str = std::to_string(line_no);
        line_t line_no_line;
        line_no_line.reserve(line_no_str.size());
        for (char c: line_no_str) {
            Glyph g(c);
            g.set_color(line_no_color);
            line_no_line.push_back(g);
        }

        // Align right by default
        pen.x = pen_start_x + line_no_rect.w - line_no_str.size() - 1;

        render_text_line(line_no_line, line_no_str.size(), pen);
        pen.y++;
    }

    const uint32_t color = Settings::const_instance().const_colors().ui;
    sdli(SDL_SetRenderDrawColor(renderer_impl_, UNWRAP_U64(color)));
    sdli(SDL_RenderDrawLine(renderer_impl_, line_no_viewport_.w-1, 0, line_no_viewport_.w-1, line_no_viewport_.h));
}

void EditorRenderer::render_info_panel(const Cursor& cursor) {
    const uint32_t text_color = Settings::const_instance().const_colors().ui;

    std::stringstream info_stream;
    info_stream << "Row: " << cursor.row() + 1 << " Column: " << cursor.col() + 1;

    line_t glyphs;
    for(char c: info_stream.str()) {
        Glyph g(c);
        g.set_color(text_color);
        glyphs.push_back(g);
    }

    const SDL_Rect& info_rect = resize_to_char_size(info_viewport_, font_width(), font_height());
    render_text_line(glyphs, glyphs.size(), {info_rect.w - static_cast<int>(glyphs.size()), 0});

    // render border line for info panel
    const uint32_t color = Settings::const_instance().const_colors().ui;
    sdli(SDL_SetRenderDrawColor(renderer_impl_, UNWRAP_U64(color)));
    sdli(SDL_RenderDrawLine(renderer_impl_, 0, 1, info_viewport_.w, 1));
}

void EditorRenderer::render_selection(const Selection& selection, const Text& text, Vec2i camera_pos) {
    if ( selection.get_state() != SELECTION_HIDDEN ) {
        const uint64_t color = Settings::const_instance().const_colors().selection;
        sdli(SDL_SetRenderDrawColor(renderer_impl_, UNWRAP_U64(color)));

        Vec2i start, finish;
        if (selection.begin().y <= selection.end().y) {
            start = selection.begin();
            finish = selection.end();
        } else {
            start = selection.end();
            finish = selection.begin();
        }
        for (int row = start.y; row <= finish.y; row++) {

            int start_col = (row == start.y)? start.x: 0;
            int fin_col = (row == finish.y)? finish.x: text.line_width(row)+1;

            Vec2i pen = camera_project_point(nullptr, {start_col, row}, camera_pos);

            int width = fin_col - start_col;
            SDL_Rect selection_rect = {
                pen.x * font_width() * FONT_SCALE,
                pen.y * font_height() * FONT_SCALE,
                width * font_width() * FONT_SCALE,
                font_height() * FONT_SCALE
            };

            sdli(SDL_RenderFillRect(renderer_impl_, &selection_rect));
        }


    }
}

void EditorRenderer::render_editor_area(const Cursor& cursor, const Text& text, const Selection& selection, Vec2i camera_pos) {
    // TODO: Scaling does not work properly with PageUp / PageDown
    // sdli(SDL_RenderSetScale(renderer_impl_, 2., 2.));

    const uint32_t color = Settings::const_instance().const_colors().bg;
    sdli(SDL_SetRenderDrawColor(renderer_impl_, UNWRAP_U64(color)));
    sdli(SDL_RenderClear(renderer_impl_));

    sdli(SDL_RenderSetViewport(renderer_impl_, &text_viewport_));
    render_selection(selection, text, camera_pos);
    render_text(text, {0,0}, camera_pos);
    render_rulers(camera_pos);
    render_cursor(cursor, text, camera_pos);

    sdli(SDL_RenderSetViewport(renderer_impl_, &line_no_viewport_));
    render_line_numbers(text, camera_pos);

    sdli(SDL_RenderSetViewport(renderer_impl_, &info_viewport_));
    render_info_panel(cursor);

    SDL_RenderPresent(renderer_impl_);
}

void EditorRenderer::set_editor_window_size(int width, int height) {
    window_rect_.x = 0;
    window_rect_.y = 0;
    window_rect_.w = width;
    window_rect_.h = height;
}

void EditorRenderer::adjust_viewports(int line_no_chars_width) {
    static const int info_viewport_height = font_height();
    line_no_viewport_ = {
        0,
        0,
        line_no_chars_width * (font_width() * FONT_SCALE),
        window_rect_.h - info_viewport_height
    };
    text_viewport_ = {
        line_no_viewport_.w,
        0,
        window_rect_.w - line_no_viewport_.w,
        window_rect_.h - info_viewport_height
    };
    info_viewport_ = {
        0,
        window_rect_.h - info_viewport_height,
        window_rect_.w,
        info_viewport_height
    };
}
