#include "editor.hpp"

#include "keyboard.hpp"
#include "settings.hpp"
#include "selection.hpp"

#include <cfenv>
#include <string>
#include <cassert>
#include <fstream>
#include <iostream>
#include <algorithm>


Editor::Editor()
    : max_line_no_chars_width_(1), camera_pos_({0, 0})
{
}

void Editor::_update_max_line_no_chars_width() {
    std::fesetround(FE_DOWNWARD);
    max_line_no_chars_width_ = std::rint(std::log10(doc_.total_lines()) + 0.00001) + 1;

    renderer_.adjust_viewports( max_line_no_chars_width_ + 1 );
}

void Editor::_adjust_cursor() {
    const Vec2i& pos = cursor_pos();
    if ( pos.y >= doc_.total_lines() ) {
        move_cursor(-pos.x, doc_.total_lines() - pos.y);
    } else {
        int line_width = doc_.line_width(pos.y);
        if ( pos.x > line_width ) {
            move_cursor(line_width - pos.x, 0);
        }
    }
}

void Editor::_set_mouse_cursor_shape(int x, int y) {
    SDL_Point mouse_pos = {x, y};
    SDL_SystemCursor system_cursor_id;
    if ( SDL_PointInRect(&mouse_pos, &text_area_rect())) {
        system_cursor_id = SDL_SYSTEM_CURSOR_IBEAM;
    } else {
        system_cursor_id = SDL_SYSTEM_CURSOR_ARROW;
    }
    SDL_Cursor* cursor = sdlp(SDL_CreateSystemCursor(system_cursor_id));
    SDL_SetCursor(cursor);
}

void Editor::set_renderer( SDL_Renderer *r, SDL_Window* w ) {
    renderer_.set_renderer(r);
    renderer_.set_window(w);

    renderer_.adjust_viewports(max_line_no_chars_width_ + 1);
}

void Editor::load_from_file(const char* filepath) {
    doc_.load_from_file(filepath);
    _update_max_line_no_chars_width();
}

void Editor::save_to_file() {
    doc_.save_to_file();
}

void Editor::move_camera(const Vec2i& cursor_pos, bool cursor_sync) {
    const SDL_Rect& text_viewport = resize_to_char_size(renderer_.text_viewport(), renderer_.font_width(), renderer_.font_height());

    Vec2i dp(0, 0);
    if (cursor_sync) {
        if (cursor_pos.x + 1 > camera_pos_.x + text_viewport.w) {
            camera_pos_.x = cursor_pos.x + 1 - text_viewport.w;
        } else if (cursor_pos.x < camera_pos_.x) {
            camera_pos_.x = cursor_pos.x;
        }

        if (cursor_pos.y + 1 > camera_pos_.y + text_viewport.h) {
            camera_pos_.y = cursor_pos.y + 1 - text_viewport.h;
        } else if (cursor_pos.y < camera_pos_.y) {
            camera_pos_.y = cursor_pos.y;
        }
    } else {
        int max_y = doc_.total_lines();
        int max_x = doc_.max_line_width();

        max_x = (max_x >= text_viewport.w)? (max_x - text_viewport.w): 0;
        max_y = (max_y >= text_viewport.h)? (max_y - text_viewport.h): 0;
        camera_pos_.x = bounded(0, max_x, cursor_pos.x);
        camera_pos_.y = bounded(0, max_y, cursor_pos.y);
    }
}

void Editor::render() {
    renderer_.render_editor_area(cursor_, doc_.text(), selection_, camera_pos_);
}

void Editor::handle_text_input(const char* text) {
    if (selection_.get_state() != SELECTION_HIDDEN) {
        remove_text(selection_.start(), selection_.finish());
        selection_.set_state(SELECTION_HIDDEN);
        move_cursor(selection_.start() - cursor_pos());
    }

    insert_text(cursor_pos(), doc_.load_raw(text));

    move_cursor({1, 0});
}


void Editor::move_cursor(int dx, int dy) {
    Vec2i begin_pos = cursor_.text_pos();

    Vec2i d = {dx, dy};
    SDL_Rect text_viewport = renderer_.text_viewport();
    Vec2i text_viewport_size = {text_viewport.w, text_viewport.h};
    cursor_.set_text_offset(d, doc_.text(), text_viewport_size);

    move_camera(cursor_.text_pos());

    Vec2i end_pos = cursor_.text_pos();

    switch ( selection_.get_state() ) {
        case SELECTION_STARTED:
            selection_.set_begin(begin_pos);
            selection_.set_end(end_pos);
            break;
        case SELECTION_IN_PROGRESS:
            selection_.set_end(end_pos);
            break;
        case SELECTION_FINISHED:
            break;
        case SELECTION_HIDDEN:
            break;
        default:
            assert(0 && "unrichable");
    }
    // std::cerr << selection_.begin() << " " << selection_.end() << " " << ss_to_cstr(selection_.get_state()) << std::endl;
}

void Editor::move_cursor(Vec2i delta) {
    move_cursor(delta.x, delta.y);
}

void Editor::set_selection_status(SelectionState s) {
    selection_.set_state(s);
}

void Editor::update_selection(bool shift_down) {
    switch ( selection_.get_state() ) {
        case SELECTION_STARTED:
            if ( shift_down ) {
                selection_.set_state(SELECTION_IN_PROGRESS);
            } else {
                selection_.set_state(SELECTION_FINISHED);
            }
            break;
        case SELECTION_IN_PROGRESS:
            if ( shift_down ) {
                // still in progress
            } else {
                selection_.set_state(SELECTION_FINISHED);
            }
            break;
        case SELECTION_HIDDEN:
            if ( shift_down ) {
                selection_.set_state(SELECTION_STARTED);
            } else {
                // do nothing
            }
            break;
        case SELECTION_FINISHED:
            if ( shift_down ) {
                selection_.set_state(SELECTION_IN_PROGRESS);
            } else {
                selection_.set_state(SELECTION_HIDDEN);
            }
            break;
        default:
            assert(0 && "unrichable");
    }
}

void Editor::selection_to_clipboard() {
    if ( selection_.get_state() == SELECTION_HIDDEN ) {
        return;
    }

    std::string selected_text = selection_.selected_text(doc_.text());
    sdli(SDL_SetClipboardText(selected_text.c_str()));
}

void Editor::insert_from_clipboard() {
    char *clipboard_text = SDL_GetClipboardText();
    if (clipboard_text) {
        Text text = Document::load_raw(clipboard_text);
        insert_text(cursor_pos(), text);
    }
    SDL_free(clipboard_text);

    _update_max_line_no_chars_width();
}

void Editor::select_all() {
    selection_.set_begin({0, 0});
    selection_.set_end({doc_.line_width(-1), doc_.total_lines() - 1});
    selection_.set_state(SELECTION_FINISHED);
}

void Editor::insert_text(const Vec2i& pos, const Text& text) {
    doc_.insert_text(pos, text);
}

void Editor::remove_text(Vec2i from, Vec2i to) {
    doc_.remove_text(from, to);
}

void Editor::goto_space(bool forward) {
    const auto& line = current_line();
    const auto& pos = cursor_pos();

    int dx = 0;
    if (forward) {  // go to the end of current word
        dx = static_cast<int>(line.size()) - pos.x;
        auto start_it = line.begin() + pos.x;
        auto first_nondelim_it = std::find_if(start_it, line.end(), is_not_word_delimiter);

        auto result = line.end();
        if ( first_nondelim_it != line.end()) {
            result = std::find_if(first_nondelim_it, line.end(), is_word_delimiter);
        }
        if (result != line.end()) {
            dx = std::distance(start_it, result);
        }
    } else {    // go to the begin of current word
        dx = -pos.x;
        auto start_it = line.rbegin() + (line.size() - pos.x);
        auto first_nondelim_it = std::find_if(start_it, line.rend(), is_not_word_delimiter);

        auto result = line.rend();
        if ( first_nondelim_it != line.rend()) {
            result = std::find_if(first_nondelim_it, line.rend(), is_word_delimiter);
        }
        if (result != line.rend()) {
            dx = std::distance(result, start_it);
        }
    }
    move_cursor({dx, 0});
}

void Editor::add_new_line() {
    if (selection_.get_state() != SELECTION_HIDDEN) {
        remove_text(selection_.start(), selection_.finish());
        selection_.set_state(SELECTION_HIDDEN);
        move_cursor(selection_.start() - cursor_pos());
    }

    const Vec2i& pos = cursor_pos();
    doc_.add_newline(pos);
    _update_max_line_no_chars_width();

    move_cursor(-pos.x, 1);

}

void Editor::handle_backspace() {
    const Vec2i& pos = cursor_pos();

    if (selection_.get_state() == SELECTION_HIDDEN) {

        if ( pos.x > 0 ) {
            remove_text(pos - Vec2i(1, 0), pos);
            move_cursor({-1, 0});
        } else {
            if ( pos.y > 0 ) {
                int prev_line_width = doc_.line_width(pos.y-1);
                doc_.remove_newline({prev_line_width, pos.y-1});
                _update_max_line_no_chars_width();
                move_cursor({prev_line_width, -1});
            }
        }
    } else {
        // delete selection
        remove_text(selection_.start(), selection_.finish());
        selection_.set_state(SELECTION_HIDDEN);
        move_cursor(selection_.start() - cursor_pos());
    }
}

void Editor::handle_delete() {
    const Vec2i& pos = cursor_pos();

    if (selection_.get_state() == SELECTION_HIDDEN) {
        if ( pos.x < doc_.line_width(pos.y) ) {
            remove_text(pos, pos + Vec2i(1, 0));
        } else {
            if ( pos.y < doc_.total_lines() - 1 ) {
                doc_.remove_newline(cursor_pos());
                _update_max_line_no_chars_width();
            }
        }
    } else {
        // delete selection
        remove_text(selection_.start(), selection_.finish());
        selection_.set_state(SELECTION_HIDDEN);
        move_cursor(selection_.start() - cursor_pos());
    }
}

void Editor::handle_keyboard_move_pressed() {
    const Uint8* keyboard = sdlp(SDL_GetKeyboardState(nullptr));

    bool shift_down = Keyboard::shift_pressed();
    bool control_down = Keyboard::ctrl_pressed();

    if (keyboard[SDL_SCANCODE_UP]) {
        update_selection(shift_down);
        move_cursor({0, -1});
    }

    if (keyboard[SDL_SCANCODE_DOWN]) {
        update_selection(shift_down);
        move_cursor({0, 1});
    }

    if (keyboard[SDL_SCANCODE_RIGHT]) {
        update_selection(shift_down);
        if ( control_down ) {
            // move to the end of the closest word from the right
            goto_space(true);
        } else {
            move_cursor({1, 0});
        }
    }

    if (keyboard[SDL_SCANCODE_LEFT]) {
        update_selection(shift_down);
        if ( control_down ) {
            // move to the end of the closest word from the left
            goto_space(false);
        } else {
            move_cursor({-1, 0});
        }
    }
}

void Editor::handle_shift_released() {
    update_selection(false);
}


void Editor::handle_return_pressed() {
    add_new_line();
}

void Editor::handle_home_pressed() {
    bool shift_down = Keyboard::shift_pressed();

    update_selection(shift_down);
    move_cursor(-cursor_pos().x, 0);
}

void Editor::handle_end_pressed() {
    bool shift_down = Keyboard::shift_pressed();

    update_selection(shift_down);
    move_cursor(current_line().size() - cursor_pos().x, 0);

}

void Editor::handle_pageup_pressed() {
    bool shift_down = Keyboard::shift_pressed();

    update_selection(shift_down);
    move_cursor({0, -text_area_char_rect().h});
}

void Editor::handle_pagedown_pressed() {
    bool shift_down = Keyboard::shift_pressed();

    update_selection(shift_down);
    move_cursor({0, text_area_char_rect().h});
}

void Editor::handle_window_size_changed(int new_width, int new_height) {
    renderer_.set_editor_window_size(new_width, new_height);
    renderer_.adjust_viewports( max_line_no_chars_width_ + 1 );
}

void Editor::handle_mouse_click(const SDL_MouseButtonEvent& event) {
    update_selection(Keyboard::shift_pressed());

    bool ok;
    Vec2i delta = _get_mouse_local_delta(ok);
    if (!ok)
        return;

    move_cursor(delta);

    if (event.clicks == 2) {
        update_selection(true);
        selection_.set_begin(cursor_pos());
        selection_.set_end(cursor_pos());
        selection_.set_state(SELECTION_IN_PROGRESS);
    }
}

void Editor::handle_mouse_button_up(const SDL_MouseButtonEvent& event) {
    UNUSED(event);

    if (selection_.get_state() == SELECTION_IN_PROGRESS) {
        update_selection(false);
    }
}

void Editor::handle_mouse_move(const SDL_MouseMotionEvent& event) {
    _set_mouse_cursor_shape(event.x, event.y);

    if (event.state & SDL_BUTTON_LMASK) {
        if (selection_.get_state() != SELECTION_HIDDEN) {
            bool ok;
            Vec2i delta = _get_mouse_local_delta(ok);
            if (!ok)
                return;

            move_cursor(delta);
            selection_.set_end(cursor_pos());
        }
    }

}

void Editor::handle_wheel(const SDL_MouseWheelEvent& wheel) {
    Vec2i diff;
    if(wheel.y > 0) {             // scroll up
        diff = {0, -1};
    } else if(wheel.y < 0) {      // scroll down
        diff = {0, 1};
    }
    if(wheel.x < 0) {             // scroll right
        diff = {1, 0};
    } else if(wheel.x > 0) {      // scroll left
        diff = {-1, 0};
    }
    move_camera(camera_pos() + diff, false);
}

Vec2i Editor::_get_mouse_local_delta(bool &success) {
    SDL_Point mouse_pos;
    Vec2i delta(0, 0);

    SDL_GetMouseState(&mouse_pos.x, &mouse_pos.y);
    if ( !SDL_PointInRect(&mouse_pos, &text_area_rect())) {
        success = false;
        return delta;
    }

    Vec2i origin = {text_area_rect().x, text_area_rect().y};
    Vec2i rel_pos = {mouse_pos.x, mouse_pos.y};

    int fw = renderer_.font_width();
    int fh = renderer_.font_height();

    delta = camera_pos_ + (rel_pos - origin) / Vec2i(fw*FONT_SCALE, fh*FONT_SCALE) - cursor_pos();
    success = true;

    return delta;
}


void Editor::log_debug_history() {
    doc_.log_items();
}


void Editor::handle_undo() {
    doc_.undo();
    _adjust_cursor();
}

void Editor::handle_redo() {
    doc_.redo();
    _adjust_cursor();
}
