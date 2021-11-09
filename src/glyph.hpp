#ifndef GLYPH_HPP_
#define GLYPH_HPP_

#include "la.hpp"

#include <string>
#include <vector>
#include <functional>


class Glyph {
public:
    explicit Glyph(const char* text);
    explicit Glyph(const char c);
    explicit Glyph();
    ~Glyph() {}

    const char* c_str() const { return ch_.c_str(); }
    const std::string& ch() const { return ch_; }

    void set_color(uint32_t color) { color_ = color; }
    const uint32_t& color() const { return color_; }

    bool operator==(const Glyph& glyph) const { return ch_ == glyph.ch(); }
private:
    std::string ch_;

    uint32_t color_;
};

std::ostream& operator<<(std::ostream& out, const Glyph& g);

bool is_word_delimiter(const Glyph& g);
bool is_not_word_delimiter(const Glyph& g);


namespace std {
    template <>
    struct hash<Glyph>
    {
        std::size_t operator()(const Glyph& k) const
        {
            return std::hash<std::string>()(k.ch());
        }
    };
} // namespace std

#endif // GLYPH_HPP_
