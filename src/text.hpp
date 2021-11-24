#ifndef TEXT_HPP_
#define TEXT_HPP_

#include "glyph.hpp"

typedef std::vector<Glyph> line_t;
typedef std::vector<line_t> content_t;

class Text {
public:
    explicit Text();
    explicit Text(const content_t& content);
    explicit Text(const content_t&& content);
    explicit Text(const Text& other);
    explicit Text(const Text&& other);

    Text& operator=(const Text& other);

    int total_lines() const { return static_cast<int>(content_.size());}
    int line_width(int row) const;
    int max_line_width() const;
    const line_t& line_at(const Vec2i& pos) const { return content_[pos.y];}

    const Vec2i get_end(const Vec2i& start) const;

    void resize(size_t nlines) { content_.resize(nlines); }

    Text& operator+=(const Text& t);
    std::pair<Text, Text> split(const Vec2i& pos);
    void insert_at(const Vec2i& pos, const Text& text);
    Text remove(const Vec2i& from, const Vec2i& to);
    void add_newline(const Vec2i& pos);
    void remove_newline(const Vec2i& pos);

    const content_t& content() const { return content_; }

    void debug(std::ostream& o);
private:
    void _recalc_max_line_width();
private:
    content_t content_;

    size_t max_line_width_;
};


#endif // TEXT_HPP_
