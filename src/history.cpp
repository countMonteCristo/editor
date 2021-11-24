#include "history.hpp"

#include "logger.hpp"
#include "document.hpp"

#include <sstream>


HistoryItem::HistoryItem(const Vec2i& pos, const Text& text, const Vec2i& cursor, bool selected)
    : pos_(pos), text_(text), selected_(selected), cursor_pos_(cursor)
{
    time_ = SDL_GetTicks();
}

History::History()
    : null_(nullptr)
{
    items_list_.push_back(pItem_t(new HeadItem()));
    list_cur_ = items_list_.begin();
}

History::~History() {
}

void History::clear() {
    items_list_.resize(1);
    list_cur_ = items_list_.begin();
}

void History::push_back(HistoryItem* item) {
    if (list_cur_ == items_list_.begin()) {
        clear();
    } else {
        if (list_cur_ != items_list_.end()) {
            items_list_.erase(std::next(list_cur_), items_list_.end());
        }
    }

    // if history is full of states - remove first element after head
    if (items_list_.size() == (max_size_ + 1)) {
        items_list_.erase(std::next(items_list_.begin()));
    }

    if (!_squash(item)) {
        items_list_.emplace_back(item);
    }
    list_cur_ = std::prev(items_list_.end());
}

const pItem_t& History::current_item() {
    if ( (list_cur_ == items_list_.begin()) || (list_cur_ == items_list_.end())) {
        return null_;
    }
    return *list_cur_;
}

const pItem_t& History::next_item() {
    if ( (items_list_.size() == 1) || (list_cur_ == std::prev(items_list_.end()))) {
        return null_;
    }
    return *std::next(list_cur_);
}

void History::log_items() {
    std::stringstream builder;
    builder << "Doc history:" << std::endl;
    size_t i = 0;
    for (auto it=items_list_.begin(); it != items_list_.end(); ++it, i++) {
        const pItem_t& item = *it;
        if (it == list_cur_) {
            builder << " -> ";
        } else {
            builder << "    ";
        }
        item->log_debug(builder);
        if (i + 1 < items_list_.size()) {
            builder << std::endl;
        }
    }
    Logger::instance().debug(builder.str());
}

bool History::_squash(const HistoryItem* item) {
    if (list_cur_ == items_list_.begin()) {
        return false;
    }
    return (*list_cur_)->squash(item);
}


void HistoryItem::_log_text(std::stringstream& builder) {
    int n_lines = text_.total_lines();
    builder << '"';
    for (int i=0; i < n_lines; i++) {
        for (const Glyph& g: text_.content()[i]) {
            builder << g;
        }
        if (i != n_lines - 1) {
            builder << "\\n";
        }
    }
    builder << '"';
}


void AddTextItem::log_debug(std::stringstream& builder) {
    builder << "AddText[pos=" << pos() << ", text=";
    _log_text(builder);
    builder << ", selected=" << selected_;
    builder << ", c=" << cursor();
    builder << "]";
}

void RemoveTextItem::log_debug(std::stringstream& builder) {
    builder << "RemoveText[pos=" << pos() << ", text=";
    _log_text(builder);
    builder << ", selected=" << selected_;
    builder << ", c=" << cursor();
    builder << "]";
}

void AddNewLineItem::log_debug(std::stringstream& builder) {
    builder << "AddNewLine[pos=" << pos();
    builder << ", selected=" << selected_;
    builder << ", c=" << cursor();
    builder << "]";
}

void RemoveNewLineItem::log_debug(std::stringstream& builder) {
    builder << "RemoveNewLine[pos=" << pos();
    builder << ", selected=" << selected_;
    builder << ", c=" << cursor();
    builder << "]";
}


void AddTextItem::undo(Document& doc) const {
    Vec2i to(text_.content().back().size(), pos_.y + text_.content().size() - 1);
    if (text_.total_lines() == 1) {
        to.x += pos_.x;
    }
    doc.remove_text(pos_, to, cursor_pos_, false, false);
}

void AddTextItem::redo(Document& doc) const {
    doc.insert_text(pos_, text_, cursor_pos_, false);
}

bool AddTextItem::squash(const HistoryItem* other) {
    if (other->created() - created() > HistoryItem::max_time_delta_ms) {
        return false;
    }
    const AddTextItem* p = dynamic_cast<const AddTextItem*>(other);
    if (!p) {
        return false;
    }

    const Text& p_text = p->text();

    // do not squash items if other item contains only space character
    if ((p_text.total_lines() == 1) && (p_text.line_width(0) == 1) && (p_text.line_at({0, 0}).front().ch() == " ")) {
        return false;
    }

    Vec2i to(pos_.x + text_.content().back().size(), pos_.y + text_.content().size() - 1);
    if (to == p->pos()) {
        text_ += p->text();
        return true;
    } else {
        return false;
    }
}

void RemoveTextItem::undo(Document& doc) const {
    doc.insert_text(pos_, text_, cursor_pos_, false);
}

void RemoveTextItem::redo(Document& doc) const {
    Vec2i to(text_.content().back().size(), pos_.y + text_.total_lines() - 1);
    if (text_.total_lines() == 1) {
        to.x += pos_.x;
    }
    doc.remove_text(pos_, to, cursor_pos_, false, false);
}

bool RemoveTextItem::squash(const HistoryItem* other) {
    if (other->created() - created() > HistoryItem::max_time_delta_ms) {
        return false;
    }
    const RemoveTextItem* p = dynamic_cast<const RemoveTextItem*>(other);
    if (!p) {
        return false;
    }

    // deleting text was like pressing delete several times
    if (pos_ == p->pos()) {
        text_ += p->text();;
        pos_ = p->pos();
        return true;
    }

    // deleting text was like pressing backspace several times
    Vec2i to(p->pos().x + p->text().content().back().size(), p->pos().y + p->text().content().size() - 1);
    if (to == pos_) {
        Text current;
        current += p->text();
        current += text_;

        text_ = current;
        pos_ = p->pos();
        return true;
    }

    return false;
}


void AddNewLineItem::undo(Document &doc) const {
    doc.remove_newline(pos_, cursor_pos_, false);
}

void AddNewLineItem::redo(Document &doc) const {
    doc.add_newline(pos_, cursor_pos_, false);
}


void RemoveNewLineItem::undo(Document &doc) const {
    doc.add_newline(pos_, cursor_pos_, false);
}

void RemoveNewLineItem::redo(Document &doc) const {
    doc.remove_newline(pos_, cursor_pos_, false);
}
