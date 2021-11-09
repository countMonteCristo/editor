#ifndef SELECTION_HPP_
#define SELECTION_HPP_

#include "la.hpp"
#include "text.hpp"

#include <cassert>


enum SelectionState {
    SELECTION_HIDDEN = 0,
    SELECTION_STARTED,
    SELECTION_IN_PROGRESS,
    SELECTION_FINISHED,
};

const char* ss_to_cstr(SelectionState s);

class Selection {
public:
    explicit Selection() : state_(SELECTION_HIDDEN) {}
    ~Selection() {}

    const Vec2i& begin() const { return begin_pos_; }
    const Vec2i& end() const { return end_pos_; }

    // start is guaranteed to be less than finish
    const Vec2i& start() const;
    const Vec2i& finish() const;

    SelectionState get_state() const { return state_; }
    void set_state(SelectionState s) { state_ = s; }

    void set_begin(const Vec2i& begin) { begin_pos_ = begin; }
    void set_end(const Vec2i& end) { end_pos_ = end; }

    std::string selected_text(const Text& lines);

private:
    Vec2i begin_pos_;
    Vec2i end_pos_;
    SelectionState state_;
};

#endif // SELECTION_HPP_
