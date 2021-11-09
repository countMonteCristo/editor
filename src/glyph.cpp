#include "glyph.hpp"

#include "settings.hpp"

#include <ostream>
#include <unordered_set>


Glyph::Glyph() {
    set_color(Settings::const_instance().const_colors().text);
}

Glyph::Glyph(const char* text) : Glyph() {
    ch_ = std::string(text);
}

Glyph::Glyph(const char c) : Glyph() {
    ch_ = std::string(1, c);
}


std::ostream& operator<<(std::ostream& out, const Glyph& g){
    return out << g.c_str();
}


bool is_word_delimiter(const Glyph& g) {
    static const std::unordered_set<char> word_delims = {
        ' ', '\t', '\n', '(', ')', '[', ']', '<', '>',
        '"', '\'', '.', ',', ':', ';', '!', '?',
        '@', '$', '%', '^', '&', '_', '|', '\\', '/', '`', '~',  '#',
        '*', '-', '+', '='
    };
    const char* cstr = g.c_str();
    return (word_delims.count(*cstr) > 0);
}

bool is_not_word_delimiter(const Glyph& g) {
    return !is_word_delimiter(g);
}
