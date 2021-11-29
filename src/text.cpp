#include "text.hpp"

#include "logger.hpp"


Text::Text() {
    content_.resize(1);
    _recalc_max_line_width();
}

Text::Text(const content_t& content)
    : content_(content)
{
    _recalc_max_line_width();
}

Text::Text(const content_t&& content)
    : content_(std::move(content))
{
    _recalc_max_line_width();
}

Text::Text(const Text& other)
    : content_(other.content()), max_line_width_(other.max_line_width())
{}

Text::Text(const Text&& other)
    : content_(std::move(other.content())), max_line_width_(other.max_line_width())
{}

Text& Text::operator=(const Text& other) {
    content_ = other.content();
    max_line_width_ = other.max_line_width();
    return *this;
}

int Text::line_width(int row) const {
    row = (row >= 0)? row: total_lines() + row;
    return static_cast<int>(content_[row].size());
}

int Text::max_line_width() const {
    if (!total_lines()) {
        return 0;
    }
    return static_cast<int>(max_line_width_);
}

const Vec2i Text::get_end(const Vec2i& start, SelectionShape shape) const {
    int last_line_index = total_lines() - 1;
    assert(last_line_index >= 0);

    int x, y;
    switch (shape) {
        case SelectionShape::TEXT_LIKE: {
            x = line_width(last_line_index);
            if (last_line_index == 0) {
                x += start.x;
            }
            y = start.y + last_line_index;
            break;
        }
        case SelectionShape::RECTANGULAR: {
            x = start.x + max_line_width();
            y = start.y + total_lines() - 1;
            break;
        }
        default:
            Logger::instance().critical(std::string("Unhandled selection shape in ") + __func__);
    }

    return Vec2i(x, y);
}

void Text::_recalc_max_line_width() {
    max_line_width_ = 0;
    for (const auto& line: content_) {
        max_line_width_ = std::max(max_line_width_, line.size());
    }
}

Text& Text::operator+=(const Text& t) {
    // if (content_.size() == 0) {
    //     content_.resize(1);
    // }
    const content_t& other = t.content();
    if ( other.size() ) {
        const line_t& first = other.front();
        line_t& back = content_.back();
        back.insert(back.end(), first.begin(), first.end());

        content_.insert(content_.end(), std::next(other.begin(), 1), other.end());
    }
    return *this;
}

std::pair<Text, Text> Text::split(const Vec2i& pos) {
    assert((pos.x >= 0) && (pos.y >= 0));

    size_t ux = static_cast<size_t>(pos.x);
    size_t uy = static_cast<size_t>(pos.y);

    if (uy >= content_.size() || ux >= content_[pos.y].size()) {
        return std::make_pair(*this, Text());
    }

    content_t first, second;
    first.reserve(pos.y + 1);
    second.reserve(content_.size() - pos.y);

    first.insert(first.begin(), content_.begin(), std::next(content_.begin(), pos.y));
    first.push_back(line_t());
    first.back().insert(first.back().begin(), content_[pos.y].begin(), std::next(content_[pos.y].begin(), pos.x));

    second.push_back(line_t());
    second.front().insert(second.front().begin(), std::next(content_[pos.y].begin(), pos.x), content_[pos.y].end());
    second.insert(std::next(second.begin(), 1), std::next(content_.begin(), pos.y+1), content_.end());

    return std::make_pair(Text(first), Text(second));
}

void Text::insert_at(const Vec2i& pos, const Text& text, SelectionShape shape) {
    const auto& raw = text.content();

    if (!raw.size()) {
        return;
    }

    switch (shape) {
        case SelectionShape::TEXT_LIKE: {
            line_t rem;
            line_t& line = content_[pos.y];

            // move text to the right of the cursor to temporary buffer
            std::move(line.begin() + pos.x, line.end(), std::back_inserter(rem));
            line.erase(line.begin() + pos.x, line.end());

            // insert first line of `text` next to cursor
            line.insert(line.end(), raw[0].begin(), raw[0].end());

            // insert all lines starting from second one
            auto it = content_.begin() + pos.y + 1;
            content_.insert(it, raw.begin()+1, raw.end());

            // insert text from tempropary buffer
            auto& last_line = content_[pos.y + raw.size() - 1];
            last_line.insert(last_line.end(), rem.begin(), rem.end());
            break;
        }
        case SelectionShape::RECTANGULAR: {
            int start_col = pos.x;
            int finish_row = pos.y + text.total_lines() - 1;
            for (int row=pos.y; row <= finish_row; row++) {
                line_t& line = content_[row];
                int actual_start_col = std::min(line_width(row)-1, start_col);
                int text_row = row - pos.y;
                line.insert(line.begin() + actual_start_col, raw[text_row].begin(), raw[text_row].end());
            }
            break;
        }
        default:
            Logger::instance().critical(std::string("Unhandled selection shape in ") + __func__);
    }


    _recalc_max_line_width();
}

Text Text::remove(const Vec2i& from, const Vec2i& to, SelectionShape shape) {
    content_t deleted_text;

    switch (shape) {
        case SelectionShape::TEXT_LIKE: {
            line_t first_deleted_line, last_deleted_line;

            int dy = to.y - from.y;

            line_t rem;
            line_t& start = content_[from.y];
            line_t& finish = content_[to.y];

            if (dy == 0) {
                std::move(finish.begin() + from.x, finish.begin() + to.x, std::back_inserter(last_deleted_line));
                finish.erase(finish.begin() + from.x, finish.begin() + to.x);
                deleted_text.push_back(last_deleted_line);
            } else {
                std::copy(start.begin() + from.x, start.end(), std::back_inserter(first_deleted_line));
                deleted_text.push_back(first_deleted_line);
                std::copy(content_.begin() + from.y + 1, content_.begin() + to.y, std::back_inserter(deleted_text));
                std::copy(finish.begin(), finish.begin() + to.x, std::back_inserter(last_deleted_line));
                deleted_text.push_back(last_deleted_line);

                std::move(finish.begin() + to.x, finish.end(), std::back_inserter(rem));
                content_.erase(content_.begin() + from.y + 1, content_.begin() + to.y + 1);
                start.erase(start.begin() + from.x, start.end());
                start.insert(start.end(), rem.begin(), rem.end());
            }
            break;
        }
        case SelectionShape::RECTANGULAR: {
            int start_col = from.x;
            int finish_col = to.x;
            if (start_col > finish_col)
                std::swap(start_col, finish_col);
            int finish_row = std::min(to.y, total_lines()-1);
            for (int row=from.y; row <= finish_row; row++) {
                line_t line;
                if (start_col >= line_width(row)) {
                    deleted_text.push_back(line);
                    continue;
                }
                int actual_finish_col = std::min(line_width(row)-1, finish_col);
                line_t& cur = content_[row];
                std::move(cur.begin() + start_col, cur.begin() + actual_finish_col, std::back_inserter(line));
                cur.erase(cur.begin() + start_col, cur.begin() + actual_finish_col);
                deleted_text.push_back(line);
            }
            break;
        }
        default:
            Logger::instance().critical(std::string("Unhandled selection shape in ") +  __func__);
    }
    _recalc_max_line_width();

    return Text(deleted_text);
}

void Text::add_newline(const Vec2i& pos) {
    line_t& current_line = content_[pos.y];

    auto line_start_it = std::next(current_line.begin(), pos.x);
    line_t movable_str = line_t(line_start_it, current_line.end());
    content_[pos.y].resize(pos.x);
    content_.insert(std::next(content_.begin(), pos.y + 1), movable_str);
}

void Text::remove_newline(const Vec2i& pos) {
    line_t& current_line = content_[pos.y];

    line_t& next_line = content_[pos.y + 1];
    current_line.insert(current_line.end(), next_line.begin(), next_line.end());
    content_.erase(std::next(content_.begin(), pos.y + 1));
}

void Text::debug(std::ostream& out) {
    for (const auto& line: content_) {
        for (const auto& g: line) {
            out << g;
        }
        out << std::endl;
    }
}
