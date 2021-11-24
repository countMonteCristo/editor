#ifndef HISTORY_HPP_
#define HISTORY_HPP_

#include "la.hpp"
#include "text.hpp"
#include "common.hpp"

#include <list>
#include <vector>
#include <memory>

#include <iostream>
#include <sstream>

class Document;

class HistoryItem;
typedef std::unique_ptr<HistoryItem> pItem_t;


class HistoryItem {
public:
    HistoryItem(const Vec2i& pos, const Text& text, bool selected);
    virtual ~HistoryItem() {}

    virtual void log_debug(std::stringstream& builder) = 0;
    virtual bool squash(const HistoryItem* other) = 0;

    virtual void undo(Document& doc) const { UNUSED(doc); std::cerr << "Not implemented!" << std::endl; };
    virtual void redo(Document& doc) const { UNUSED(doc); std::cerr << "Not implemented!" << std::endl; }

    uint32_t created() const {return time_;}

    const Vec2i& pos() const { return pos_; }
    const Vec2i end() const { return text_.get_end(pos_); }
    const Text& text() const { return text_; }

    bool selected() const { return selected_; }
protected:
    void _log_text(std::stringstream& builder);
protected:
    Vec2i pos_;
    Text text_;
    uint32_t time_;
    bool selected_;

    static const uint32_t max_time_delta_ms = 1000;
};

class HeadItem: public HistoryItem {
public:
    HeadItem() : HistoryItem(Vec2i(0, 0), Text(), false) {}
    virtual void log_debug(std::stringstream& builder) { builder << "HeadItem[]"; }
    virtual bool squash(const HistoryItem*) { return false; }
};

class AddTextItem: public HistoryItem {
public:
    AddTextItem(const Vec2i& pos, const Text& text) : HistoryItem(pos, text, false) {}
    virtual ~AddTextItem() {}

    virtual void undo(Document& doc) const;
    virtual void redo(Document& doc) const;

    virtual bool squash(const HistoryItem* other);

    virtual void log_debug(std::stringstream& builder);
private:

};

class AddNewLineItem: public HistoryItem {
public:
    AddNewLineItem(const Vec2i& pos) : HistoryItem(pos, Text(), false) {}
    virtual ~AddNewLineItem() {}

    virtual void undo(Document& doc) const;
    virtual void redo(Document& doc) const;

    virtual void log_debug(std::stringstream& builder);
    virtual bool squash(const HistoryItem*) { return false; }
private:
};

class RemoveNewLineItem: public HistoryItem {
public:
    RemoveNewLineItem(const Vec2i& pos) : HistoryItem(pos, Text(), false) {}
    virtual ~RemoveNewLineItem() {}

    virtual void undo(Document& doc) const;
    virtual void redo(Document& doc) const;

    virtual void log_debug(std::stringstream& builder);
    virtual bool squash(const HistoryItem*) { return false; }
private:

};

class RemoveTextItem: public HistoryItem {
public:
    RemoveTextItem(const Vec2i& pos, const Text& text, bool selected) : HistoryItem(pos, text, selected) {}
    virtual ~RemoveTextItem() {}

    virtual void undo(Document& doc) const;
    virtual void redo(Document& doc) const;

    virtual void log_debug(std::stringstream& builder);

    virtual bool squash(const HistoryItem*); /*{ return false; }*/
private:

};


class History {
public:
    History();
    ~History();

    void push_back(HistoryItem* item);
    const pItem_t& current_item();
    const pItem_t& next_item();

    void dec() { list_cur_ = (list_cur_ == items_list_.begin())? list_cur_: std::prev(list_cur_); }
    void inc() { list_cur_ = (list_cur_ == std::prev(items_list_.end())? list_cur_: std::next(list_cur_)); }

    int size() const { return static_cast<int>(items_list_.size());}
    void clear();

    void log_items();
private:
    bool _squash(const HistoryItem* item);
private:
    std::list<pItem_t> items_list_;
    std::list<pItem_t>::iterator list_cur_;

    const pItem_t null_;
    const size_t max_size_ = 1024;  // should always be >= 0
};

#endif // HISTORY_HPP_
