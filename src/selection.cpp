#include "selection.hpp"

#include "logger.hpp"

#include <sstream>


const char* ss_to_cstr(SelectionState s)
{
    switch (s) {
        case SelectionState::HIDDEN:
            return "HIDDEN";
        case SelectionState::STARTED:
            return "STARTED";
        case SelectionState::IN_PROGRESS:
            return "IN_PROGRESS";
        case SelectionState::FINISHED:
            return "FINISHED";
        default:
            std::stringstream builder;
            builder << "Unhandled SelectionState value at ss_to_cstr(): " << static_cast<int>(s);
            Logger::instance().critical(builder.str());
            return "";
    }
}


// TODO: toggle selection state by pressing alt
// TODO: think how to properly delete rectanglurly selected text
std::string Selection::selected_text(const Text& text) {
    Vec2i sel_start = start();
    Vec2i sel_finish = finish();

    std::string data;
    switch (get_shape()) {
        case SelectionShape::TEXT_LIKE: {
            for (int row = sel_start.y; row <= sel_finish.y; row++) {
                int start_col = (row == sel_start.y)? sel_start.x: 0;
                int finish_col = (row == sel_finish.y)? sel_finish.x: text.line_width(row);

                for (int col = start_col; col < finish_col; col++) {
                    data += std::string(text.line_at({0, row})[col].c_str());
                }
                if (row < sel_finish.y) {
                    data += '\n';
                }
            }
            break;
        }
        case SelectionShape::RECTANGULAR: {
            for (int row = sel_start.y; row <= sel_finish.y; row++) {
                int start_col = sel_start.x;
                int finish_col = sel_finish.x;

                if (start_col >= finish_col)
                    std::swap(start_col, finish_col);

                for (int col = start_col; col < finish_col; col++) {
                    if (col < text.line_width(row))
                        data += std::string(text.line_at({0, row})[col].c_str());
                    else
                        data += std::string(" ");
                }
                if (row < sel_finish.y) {
                    data += '\n';
                }
            }
            break;
        }
        default:
            Logger::instance().critical("Cannot get selected text of unknown selection shape");
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
