#include "greatest.h"

#include <list.h>
#include <text.h>

static Text text_;
static Text* text;

TEST cstring_to_text_should_handle_ascii_strings() {
    ASSERT(cstring_to_text(text, "hello"));
    ASSERT_EQ_FMT(5, text->count, "%d");

    PASS();
}

/* Handle a 2 character UTF-8 string. */
TEST cstring_to_text_should_handle_latin() {
    ASSERT(cstring_to_text(text, "łódź"));
    ASSERT_EQ_FMT(4, text->count, "%d");

    PASS();
}

/* Handle a 3 character UTF-8 string. */
TEST cstring_to_text_should_handle_bmp() {
    ASSERT(cstring_to_text(text, "働"));
    ASSERT_EQ_FMT(1, text->count, "%d");

    PASS();
}

/* Handle a 4 character UTF-8 string. */
TEST cstring_to_text_should_handle_astral_code_points() {
    /* You could say I'm a flan of this test case. */
    ASSERT(cstring_to_text(text, "🍮"));
    ASSERT_EQ_FMT(1, text->count, "%d");

    PASS();
}

static void initialize_text(void *unused) {
    text = &text_;
    list_initialize(text);
}

static void deinitialize_text(void *unused) {
    /* Frees each character and clears the text. */
    list_empty(text, free);
}

SUITE(text_suite) {
    SET_SETUP(initialize_text, NULL);
    SET_TEARDOWN(deinitialize_text, NULL);

    RUN_TEST(cstring_to_text_should_handle_ascii_strings);
    RUN_TEST(cstring_to_text_should_handle_latin);
    RUN_TEST(cstring_to_text_should_handle_bmp);
    RUN_TEST(cstring_to_text_should_handle_astral_code_points);
}
