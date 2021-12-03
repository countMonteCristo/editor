#include "glyph.hpp"

#include "settings.hpp"

#include <ostream>
#include <unordered_set>


Glyph::Glyph() {
    set_color(Settings::const_instance().const_colors().text);
}

Glyph::Glyph(const char* visible_text) : Glyph() {
    if (visible_text)
        visible_ch_ = std::string(visible_text);
}

Glyph::Glyph(const char* real_text, const char* visible_text) : Glyph() {
    real_ch_ = std::string(real_text);

    if (visible_text)
        visible_ch_ = std::string(visible_text);
    else
        visible_ch_ = real_ch_;
}

Glyph& Glyph::operator=(const Glyph& g) {
    real_ch_ = g.real();
    visible_ch_ = g.visible();
    color_ = g.color();
    return *this;
}


std::ostream& operator<<(std::ostream& out, const Glyph& g) {
    return out << g.real().c_str();
}


bool is_word_delimiter(const Glyph& g) {
    static const std::unordered_set<char> word_delims = {
        ' ', '\t', '\n', '(', ')', '[', ']', '<', '>',
        '"', '\'', '.', ',', ':', ';', '!', '?',
        '@', '$', '%', '^', '&', '_', '|', '\\', '/', '`', '~',  '#',
        '*', '-', '+', '='
    };
    const char* cstr = g.real().c_str();
    return (word_delims.count(*cstr) > 0);
}

bool is_not_word_delimiter(const Glyph& g) {
    return !is_word_delimiter(g);
}
