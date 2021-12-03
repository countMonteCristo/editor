#ifndef GLYPH_HPP_
#define GLYPH_HPP_

#include "la.hpp"

#include <string>
#include <vector>
#include <functional>
#include <unordered_map>


class Glyph {
public:
    explicit Glyph(const char* real_text, const char* visible_text);
    explicit Glyph();

    Glyph& operator=(const Glyph& other);
    ~Glyph() {}

    const std::string& real() const { return real_ch_; }
    const std::string& visible() const { return visible_ch_; }

    void set_color(uint32_t color) { color_ = color; }
    const uint32_t& color() const { return color_; }

    bool operator==(const Glyph& glyph) const { return real_ch_ == glyph.real(); }
private:
    std::string real_ch_;

    std::string visible_ch_;

    uint32_t color_;
};

std::ostream& operator<<(std::ostream& out, const Glyph& g);

bool is_word_delimiter(const Glyph& g);
bool is_not_word_delimiter(const Glyph& g);

typedef std::unordered_map<char, Glyph> char_map_t;

namespace std {
    template <>
    struct hash<Glyph>
    {
        std::size_t operator()(const Glyph& k) const
        {
            return std::hash<std::string>()(k.real());
        }
    };
} // namespace std

#endif // GLYPH_HPP_
