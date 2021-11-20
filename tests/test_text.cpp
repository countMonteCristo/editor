#include <gtest/gtest.h>

#include "text.hpp"
#include "glyph.hpp"
#include "document.hpp"


class GlyphFixture: public ::testing::Test {
protected:
    void SetUp() override {
        g1 = Glyph();
        g2 = Glyph('q');
        g3 = Glyph("Ы");
    }

    Glyph g1;
    Glyph g2;
    Glyph g3;
};

class TextFixture: public ::testing::Test {
protected:
    void SetUp() override {
        empty_text = Text();

        text_from_content = Document::load_raw("print_if_you_want\nHello\nWorld!");
        text_from_text = Text(text_from_content);
    }

    Text empty_text;
    Text text_from_content;
    Text text_from_text;
};

TEST_F(GlyphFixture, ValidGlyph) {
    EXPECT_STREQ(g1.c_str(), "");
    EXPECT_STREQ(g2.c_str(), "q");
    EXPECT_STREQ(g3.c_str(), "Ы");

    EXPECT_EQ(g1.ch().size(), 0);
    EXPECT_EQ(g2.ch().size(), 1);
    EXPECT_EQ(g3.ch().size(), 2);

    uint32_t color = 0xabcdef12;
    g1.set_color(color);
    EXPECT_EQ(g1.color(), color);

    std::hash<Glyph> glyph_hash;
    size_t hash_value = glyph_hash(g3);
    EXPECT_EQ(hash_value, std::hash<std::string>()(g3.ch()));
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
    EXPECT_STREQ(text_from_content.line_at({6, 2}).at(6).c_str(), "p");
}
