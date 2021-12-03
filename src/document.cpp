#include "document.hpp"

#include "common.hpp"
#include "logger.hpp"
#include "settings.hpp"

#include <cassert>
#include <fstream>


Document::Document() : max_line_width_(0), filepath_("out.txt") {
    _init_special_chars();
}

line_t Document::load_line(const char* start, const char* stop) {
    line_t glyphs;

    char buf[3] = {0};
    const char* pos = start;
    while (pos < stop) {
        char c = *pos;
        Glyph g;

        auto it = special_chars_.find(c);
        if ( it != special_chars_.end() ) {
            g = it->second;
            pos++;
        } else {
            buf[0] = c;
            if ( (c >= ASCII_CHAR_LOW) && (c <= ASCII_CHAR_HIGH) ) {
                buf[1] = 0;
                pos++;
            } else {
                buf[1] = *(pos+1);
                pos += 2;
            }
            g = Glyph(buf, nullptr);
            g.set_color(Settings::const_instance().const_colors().text);
        }
        glyphs.push_back(g);
    }

    return glyphs;
}

Text Document::load_raw(const char *start) {
    content_t content;
    const char* cur = start;
    const char* pos = start;
    while (true) {
        if ((*cur == '\n') || (*cur == '\0')) {
            content.push_back(Document::load_line(pos, cur));
            if (*cur == '\0')
                break;
            pos = cur+1;
            cur = pos;
            continue;
        }
        cur++;
    }
    return Text(content);
}

void Document::load_from_file(const std::string& filepath) {
    filepath_ = filepath;

    std::ifstream infile(filepath_);
    if ( !infile.is_open() ) {
        Logger::instance().error("Cannot load file: " + filepath_);
        return;
    }

    std::string line;
    content_t content;

    while (std::getline(infile, line))
    {
        line_t glyphs = Document::load_line(line.c_str(), line.c_str() + line.size());
        content.push_back(glyphs);
    }
    infile.close();

    text_ = Text(content);
}

void Document::save_to_file() {
    std::ofstream outfile(filepath_.c_str());
    if ( !outfile.is_open() ) {
        Logger::instance().error("Cannot save to file: " + filepath_);
        return;
    }

    for (const auto& line: text_.content()) {
        for (const Glyph& g: line) {
            outfile << g.real().c_str();
        }
        outfile << std::endl;
    }
    outfile.close();
}


void Document::insert_glyph(const Vec2i& pos, const Glyph& glyph, const Vec2i& cursor, SelectionShape shape, bool remember) {
    content_t content;
    content.resize(1);
    content[0].push_back(glyph);
    Text text(content);
    insert_text(pos, text, cursor, shape, remember);
}

void Document::insert_text(const Vec2i& pos, const Text& text, const Vec2i& cursor, SelectionShape shape, bool remember) {
    if (remember) {
        AddTextItem* item = new AddTextItem(pos, text, cursor, shape);
        history_.push_back(item);
    }
    text_.insert_at(pos, text, shape);
}

void Document::remove_text(Vec2i from, Vec2i to, const Vec2i& cursor, SelectionShape shape, bool remember) {
    if ( (from.y > to.y) || ((from.y == to.y) && (from.x > to.x)) ) {
        std::swap(from, to);
    }

    Text removed = text_.remove(from, to, shape);

    if (remember) {
        RemoveTextItem* item = new RemoveTextItem(from, removed, cursor, shape);
        history_.push_back(item);
    }
}

void Document::add_newline(const Vec2i& pos, const Vec2i& cursor, bool remember) {
    text_.add_newline(pos);

    if (remember) {
        AddNewLineItem *item = new AddNewLineItem(pos, cursor);
        history_.push_back(item);
    }
}

void Document::remove_newline(const Vec2i& pos, const Vec2i& cursor, bool remember) {
    text_.remove_newline(pos);

    if (remember) {
        RemoveNewLineItem *item = new RemoveNewLineItem(pos, cursor);
        history_.push_back(item);
    }
}


// TODO: add setting for special chars color
void Document::_init_special_chars() {
    // tab -> →
    char buf[4] = {
        static_cast<char>(0xe2), static_cast<char>(0x86), static_cast<char>(0x92), 0
    };
    Glyph g("\t", buf);
    g.set_color(0x888888ff);
    special_chars_['\t'] = g;
}

void Document::log_items() {
    history_.log_items();
}


const pItem_t& Document::undo() {
    const pItem_t& item = history_.current_item();
    if (item) {
        item->undo(*this);
        history_.dec();
    }
    return item;
}

const pItem_t& Document::redo() {
    const pItem_t& item = history_.next_item();
    if (item) {
        item->redo(*this);
        history_.inc();
    }
    return item;
}
