#include "document.hpp"

#include "common.hpp"
#include "logger.hpp"
#include "settings.hpp"

#include <cassert>
#include <fstream>


Document::Document() : max_line_width_(0), filepath_("out.txt") {
}

line_t Document::load_line(const char* start, const char* stop) {
    line_t glyphs;

    char buf[2];
    const char* pos = start;
    while (pos < stop) {
        char c = *pos;
        Glyph g;
        if ( (c >= ASCII_CHAR_LOW) && (c <= ASCII_CHAR_HIGH) ) {
            g = Glyph(c);
            pos++;
        } else {
            buf[0] = c;
            buf[1] = *(pos+1);
            pos += 2;
            g = Glyph(buf);
        }
        g.set_color(Settings::const_instance().const_colors().text);
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
            outfile << g.c_str();
        }
        outfile << std::endl;
    }
    outfile.close();
}


void Document::insert_text(const Vec2i& pos, const Text& text, const Vec2i& cursor, bool remember) {
    if (remember) {
        AddTextItem* item = new AddTextItem(pos, text, cursor);
        history_.push_back(item);
    }
    text_.insert_at(pos, text);
}

void Document::remove_text(Vec2i from, Vec2i to, const Vec2i& cursor, bool selected, bool remember) {
    if ( (from.y > to.y) || ((from.y == to.y) && (from.x > to.x)) ) {
        std::swap(from, to);
    }

    Text removed = text_.remove(from, to);

    if (remember) {
        RemoveTextItem* item = new RemoveTextItem(from, removed, cursor, selected);
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
