#include <gtest/gtest.h>

#include "text.hpp"
#include "glyph.hpp"
#include "document.hpp"


// TODO: add tests for special characters
// TODO: add tests for real and visible text
class GlyphFixture: public ::testing::Test {
protected:
    void SetUp() override {
        g1 = Glyph();
        g2 = Glyph("q", nullptr);
        g3 = Glyph("Ы", nullptr);
    }

    Glyph g1;
    Glyph g2;
    Glyph g3;
};

class TextFixture: public ::testing::Test {
protected:
    void SetUp() override {
        empty_text = Text();
        Document doc;

        text_from_content = doc.load_raw("print_if_you_want\nHello\nWorld!");
        text_from_text = Text(text_from_content);
    }

    Text empty_text;
    Text text_from_content;
    Text text_from_text;
};

TEST_F(GlyphFixture, ValidGlyph) {
    EXPECT_STREQ(g1.real().c_str(), "");
    EXPECT_STREQ(g2.real().c_str(), "q");
    EXPECT_STREQ(g3.real().c_str(), "Ы");

    EXPECT_EQ(g1.real().size(), 0);
    EXPECT_EQ(g2.real().size(), 1);
    EXPECT_EQ(g3.real().size(), 2);

    uint32_t color = 0xabcdef12;
    g1.set_color(color);
    EXPECT_EQ(g1.color(), color);

    std::hash<Glyph> glyph_hash;
    size_t hash_value = glyph_hash(g3);
    EXPECT_EQ(hash_value, std::hash<std::string>()(g3.real()));
}

TEST_F(TextFixture, TextInit) {
    EXPECT_EQ(empty_text.total_lines(), 1);
    EXPECT_EQ(text_from_content.total_lines(), 3);
    EXPECT_EQ(text_from_text.total_lines(), 3);

    EXPECT_EQ(text_from_content.line_width(1), 5);
    EXPECT_EQ(text_from_text.max_line_width(), 17);

    EXPECT_EQ(text_from_text.line_at({21, 0}).size(), 17);
}

TEST_F(TextFixture, TextChange) {
    int text_from_content_lines = text_from_content.total_lines();
    text_from_content += text_from_text;

    EXPECT_EQ(text_from_content.total_lines(), text_from_content_lines + text_from_text.total_lines() - 1);
    EXPECT_STREQ(text_from_content.line_at({6, 2}).at(6).real().c_str(), "p");
}

TEST_F(TextFixture, TextSplit) {
    Vec2i pos = {2, 1};

    const auto& last_line = text_from_content.line_at({0, text_from_content.total_lines()-1});
    int last_line_width = text_from_content.line_width(text_from_content.total_lines()-1);

    const char* first_char = text_from_content.line_at({0, 0}).at(0).real().c_str();
    const char* last_char = last_line.at(last_line_width-1).real().c_str();
    const char* split_char = text_from_content.line_at(pos).at(pos.x).real().c_str();
    const char* prev_char = text_from_content.line_at(pos).at(pos.x-1).real().c_str();
    auto pair = text_from_content.split(pos);
    EXPECT_EQ(pair.first.total_lines(), pos.y + 1);
    EXPECT_EQ(pair.second.total_lines(), text_from_content.total_lines() - pos.y);

    EXPECT_STREQ(pair.first.line_at({0, 0}).at(0).real().c_str(), first_char);
    EXPECT_STREQ(pair.first.line_at(pos).at(pos.x-1).real().c_str(), prev_char);
    EXPECT_STREQ(pair.second.line_at({0, 0}).at(0).real().c_str(), split_char);
    EXPECT_STREQ(pair.second.line_at({last_line_width-1, 1}).at(last_line_width-1).real().c_str(), last_char);
}

TEST_F(TextFixture, TextInsertMultiLine) {
    Vec2i pos = {2, 1};
    Document doc;
    Text inserted = doc.load_raw("load from\nanother file");

    const char* inserted_first_char = inserted.line_at({0, 0}).at(0).real().c_str();

    auto& text = text_from_content;
    int original_lines_count = text.total_lines();

    int inserted_line_width = text.line_width(pos.y);

    text.insert_at(pos, inserted, SelectionShape::TEXT_LIKE);

    // total_lines shoul be equal to (#_of_\n + 1)
    EXPECT_EQ(text.total_lines(), original_lines_count + inserted.total_lines() - 1);

    // char at split position should be equal to the first char of the inserted text
    EXPECT_STREQ(text.line_at(pos).at(pos.x).real().c_str(), inserted_first_char);

    // width of the line at insertion_pos.y should be equal to inserted_line_width + inserted.first_line_width
    EXPECT_EQ(text.line_width(pos.y), pos.x + inserted.line_width(0));

    // width of the new line where insertion ends should be equal to inserted.last_line_width + inserted_line_width - pos.x
    EXPECT_EQ(text.line_width(pos.y+inserted.total_lines()-1), inserted.line_width(inserted.total_lines()-1) + inserted_line_width - pos.x);
}

TEST_F(TextFixture, TestInsertSingleLine) {
    Vec2i pos = {2, 1};
    Document doc;
    Text inserted = doc.load_raw("load from");

    const char* inserted_first_char = inserted.line_at({0, 0}).at(0).real().c_str();

    auto& text = text_from_content;
    int original_lines_count = text.total_lines();

    int inserted_line_width = text.line_width(pos.y);

    text.insert_at(pos, inserted, SelectionShape::TEXT_LIKE);

    // total_lines shoul be equal to (#_of_\n + 1)
    EXPECT_EQ(text.total_lines(), original_lines_count + inserted.total_lines() - 1);

    // char at split position should be equal to the first char of the inserted text
    EXPECT_STREQ(text.line_at(pos).at(pos.x).real().c_str(), inserted_first_char);

    // width of the line at insertion_pos.y should be equal to inserted_line_width + inserted.first_line_width
    EXPECT_EQ(text.line_width(pos.y), inserted.line_width(0) + inserted_line_width);
}

TEST_F(TextFixture, TestRemoveSingleLine) {
    Vec2i from = {3, 0};
    Vec2i to = {10, 0};

    Text& text = text_from_content;
    int original_line_width_at_removing_from = text.line_width(from.y);
    int original_total = text.total_lines();

    std::string first_removed = text.line_at(from).at(from.x).real();
    std::string last_removed = text.line_at(to).at(to.x-1).real();

    Text removed = text.remove(from, to, SelectionShape::TEXT_LIKE);

    EXPECT_EQ(text.total_lines(), original_total - (to.y - from.y));
    EXPECT_EQ(text.line_width(from.y), original_line_width_at_removing_from - (to.x - from.x));

    EXPECT_EQ(removed.total_lines(), 1);
    EXPECT_EQ(removed.line_width(0), to.x - from.x);

    EXPECT_STREQ(first_removed.c_str(), removed.line_at({0, 0}).at(0).real().c_str());
    EXPECT_STREQ(last_removed.c_str(), removed.line_at({0, 0}).at(removed.line_width(0)-1).real().c_str());
}

TEST_F(TextFixture, TestRemoveMultiLine) {
    Vec2i from = {3, 0};
    Vec2i to = {2, 1};

    Text& text = text_from_content;

    int original_line_width_at_removing_from = text.line_width(from.y);
    int original_line_width_at_removing_to = text.line_width(to.y);
    int original_total = text.total_lines();

    std::string first_removed = text.line_at(from).at(from.x).real();
    std::string last_removed = text.line_at(to).at(to.x-1).real();

    Text removed = text.remove(from, to, SelectionShape::TEXT_LIKE);
    int removed_total = to.y - from.y + 1;

    EXPECT_EQ(text.total_lines(), original_total - (to.y - from.y));
    EXPECT_EQ(text.line_width(from.y), original_line_width_at_removing_to - to.x + from.x);

    EXPECT_EQ(removed.total_lines(), removed_total);
    EXPECT_EQ(removed.line_width(0), original_line_width_at_removing_from - from.x);

    EXPECT_STREQ(first_removed.c_str(), removed.line_at({0, 0}).at(0).real().c_str());
    EXPECT_STREQ(last_removed.c_str(), removed.line_at({0, removed.total_lines()-1}).at(removed.line_width(removed.total_lines()-1)-1).real().c_str());
}

TEST_F(TextFixture, TextAddNewLine) {
    Vec2i pos = {2, 1};

    Text& text = text_from_content;

    int original_line_width_at_new_newline = text.line_width(pos.y);
    int original_total = text.total_lines();

    text.add_newline(pos);

    EXPECT_EQ(text.total_lines(), original_total + 1);

    EXPECT_EQ(text.line_width(pos.y), pos.x);
    EXPECT_EQ(text.line_width(pos.y+1), original_line_width_at_new_newline - pos.x);
}

TEST_F(TextFixture, TextRemoveNewLine) {
    Vec2i pos = {0, 1};

    Text& text = text_from_content;

    int original_line_width_before_new_newline = text.line_width(pos.y);
    int original_line_width_after_new_newline = text.line_width(pos.y+1);
    int original_total = text.total_lines();

    text.remove_newline(pos);

    EXPECT_EQ(text.total_lines(), original_total - 1);
    EXPECT_EQ(text.line_width(pos.y), original_line_width_before_new_newline + original_line_width_after_new_newline);
}
