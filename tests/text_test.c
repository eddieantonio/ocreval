#include "greatest.h"

#include <text.h>

TEST cstring_to_text_should_handle_ascii_strings() {
    Text text = { 0 };

    ASSERT_EQ_FMT(0, text.count, "%d");
    ASSERT(cstring_to_text(&text, "hello"));
    ASSERT_EQ_FMT(5, text.count, "%d");

    PASS();
}

/* Handle a 2 character UTF-8 string. */
TEST cstring_to_text_should_handle_latin() {
    Text text = { 0 };

    ASSERT_EQ_FMT(0, text.count, "%d");
    ASSERT(cstring_to_text(&text, "łódź"));
    ASSERT_EQ_FMT(4, text.count, "%d");

    PASS();
}

/* Handle a 3 character UTF-8 string. */
TEST cstring_to_text_should_handle_bmp() {
    Text text = { 0 };

    ASSERT_EQ_FMT(0, text.count, "%d");
    ASSERT(cstring_to_text(&text, "働"));
    ASSERT_EQ_FMT(1, text.count, "%d");

    PASS();
}

SUITE(text_suite) {
    RUN_TEST(cstring_to_text_should_handle_ascii_strings);
    RUN_TEST(cstring_to_text_should_handle_latin);
    RUN_TEST(cstring_to_text_should_handle_bmp);
}
