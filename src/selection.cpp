#include "selection.hpp"

#include "logger.hpp"

#include <sstream>


const char* ss_to_cstr(SelectionState s)
{
    switch (s) {
        case SELECTION_HIDDEN:
            return "HIDDEN";
        case SELECTION_STARTED:
            return "STARTED";
        case SELECTION_IN_PROGRESS:
            return "IN_PROGRESS";
        case SELECTION_FINISHED:
            return "FINISHED";
        default:
            std::stringstream builder;
            builder << "Unhandled SelectionState value at ss_to_cstr(): " << static_cast<int>(s);
            Logger::instance().critical(builder.str());
            return "";
    }
}

std::string Selection::selected_text(const Text& text) {
    Vec2i start, finish;
    if ( begin().y <= end().y) {
        start = begin();
        finish = end();
    } else {
        start = end();
        finish = begin();
    }

    std::string data;
    for (int row = start.y; row <= finish.y; row++) {
        int start_col = (row == start.y)? start.x: 0;
        int finish_col = (row == finish.y)? finish.x: text.line_width(row);

        for (int col = start_col; col < finish_col; col++) {
            data += std::string(text.line_at({0, row})[col].c_str());
        }
        if (row < finish.y) {
            data += '\n';
        }
    }

    return data;
}


const Vec2i& Selection::start() const {
    if ((begin_pos_.y < end_pos_.y) || ((begin_pos_.y == end_pos_.y) && (begin_pos_.x <= end_pos_.x)))
        return begin_pos_;
    return end_pos_;
}

const Vec2i& Selection::finish() const {
    if ((begin_pos_.y < end_pos_.y) || ((begin_pos_.y == end_pos_.y) && (begin_pos_.x <= end_pos_.x)))
        return end_pos_;
    return begin_pos_;
}
