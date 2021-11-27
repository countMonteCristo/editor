#ifndef DOCUMENT_HPP_
#define DOCUMENT_HPP_

#include "text.hpp"
#include "la.hpp"
#include "history.hpp"


constexpr char ASCII_CHAR_LOW = 32;
constexpr char ASCII_CHAR_HIGH = 126;

class Document {
public:
    Document();
    ~Document() {}

    const line_t& line_at(const Vec2i& pos) const { return text_.line_at(pos); }
    int total_lines() const { return text_.total_lines(); }
    int line_width(int row) const { return text_.line_width(row); }
    int max_line_width() const { return text_.max_line_width(); }
    const Text& text() const { return text_; }
    const std::string& filepath() const { return filepath_; }

    static line_t load_line(const char* start, const char* stop);
    static Text load_raw(const char* start);

    void load_from_file(const std::string& filepath);
    void save_to_file();

    void insert_text(const Vec2i& pos, const Text& text, const Vec2i& cursor, SelectionShape shape, bool remember=true);
    void remove_text(Vec2i from, Vec2i to, const Vec2i& cursor, SelectionShape shape, bool remember=true);
    void add_newline(const Vec2i& pos, const Vec2i& cursor, bool remember=true);
    void remove_newline(const Vec2i& pos, const Vec2i& cursor, bool remember=true);

    void log_items();

    const pItem_t& undo();
    const pItem_t& redo();
private:
    void _recalc_max_line_width();

private:
    Text text_;
    History history_;

    size_t max_line_width_;

    std::string filepath_;
};

#endif // DOCUMENT_HPP_
