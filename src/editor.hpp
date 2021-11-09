#ifndef EDITOR_HPP_
#define EDITOR_HPP_

#include <vector>
#include <string>
#include <list>
#include <unordered_map>

#include <SDL2/SDL.h>

#include "la.hpp"
#include "text.hpp"
#include "common.hpp"
#include "cursor.hpp"
#include "renderer.hpp"
#include "document.hpp"
#include "selection.hpp"

#include "history.hpp"


class Editor {
public:
    explicit Editor();
    ~Editor() {}

    void set_renderer( SDL_Renderer* r, SDL_Window* w);

    void load_from_file(const char* filepath);
    void save_to_file();

    void move_cursor(int dx, int dy);
    void move_cursor(Vec2i delta);
    void move_camera(const Vec2i& cursor_pos, bool cursor_sync=true);
    void render();

    void set_selection_status(SelectionState s);
    void update_selection(bool shift_down);
    void selection_to_clipboard();
    void insert_from_clipboard();
    void select_all();

    void insert_text(const Vec2i& pos, const Text& text);
    void remove_text(Vec2i from, Vec2i to);

    inline const Vec2i& cursor_pos() const {return cursor_.text_pos();}
    inline const Vec2i& camera_pos() const { return camera_pos_; }

    const SDL_Rect& text_area_rect() const { return renderer_.text_viewport(); }
    const SDL_Rect text_area_char_rect() const { return resize_to_char_size(renderer_.text_viewport(), renderer_.font_width(), renderer_.font_height()); }

    const line_t& current_line() { return doc_.line_at(cursor_pos()); }
    const std::string& filepath() const { return doc_.filepath(); }

    void log_debug_history();

    void add_new_line();

    void handle_text_input(const char* text);
    void handle_backspace();
    void handle_delete();
    void handle_keyboard_move_pressed();
    void handle_shift_released();
    void handle_return_pressed();
    void handle_home_pressed();
    void handle_end_pressed();
    void handle_pageup_pressed();
    void handle_pagedown_pressed();
    void handle_window_size_changed(int new_width, int new_height);
    void handle_mouse_click(const SDL_MouseButtonEvent& event);
    void handle_mouse_button_up(const SDL_MouseButtonEvent& event);
    void handle_mouse_move(const SDL_MouseMotionEvent& event);
    void handle_wheel(const SDL_MouseWheelEvent& wheel);
    void handle_undo();
    void handle_redo();

    void goto_space(bool forward);
private:
    void _update_max_line_no_chars_width();
    void _adjust_cursor();
    void _set_mouse_cursor_shape(int x, int y);

protected:
    Vec2i _get_mouse_local_delta(bool &success);

private:
    Document doc_;
    int max_line_no_chars_width_;

    Vec2i camera_pos_;

    Cursor cursor_;
    EditorRenderer renderer_;
    Selection selection_;
};

#endif // EDITOR_HPP_
