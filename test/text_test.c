#include "greatest.h"
#include "test_utils.h"

#include <list.h>
#include <text.h>

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

TEST char_to_string_converts_space() {
    char buffer[STRING_SIZE];

    char_to_string(False, ' ', buffer, False);
    ASSERT_STR_EQ(" ", buffer);

    PASS();
}

TEST char_to_string_converts_printable_ascii() {
    char buffer[STRING_SIZE];

    char_to_string(False, '%', buffer, False);
    ASSERT_STR_EQ("%", buffer);

    PASS();
}

TEST char_to_string_converts_non_printable_ascii() {
    char buffer[STRING_SIZE];

    char_to_string(False, 0x0007, buffer, False);
    ASSERT_STR_EQ("<07>", buffer);

    PASS();
}

TEST char_to_string_converts_printable_bmp() {
    char buffer[STRING_SIZE];

    char_to_string(False, 0x50cd, buffer, False);
    ASSERT_STR_EQ("働", buffer);

    PASS();
}

TEST char_to_string_converts_astral_code_points() {
    char buffer[STRING_SIZE];

    char_to_string(False, 0x0101E1, buffer, False);
    ASSERT_STR_EQ("𐇡", buffer);

    PASS();
}

TEST char_to_string_converts_bmp_combiner() {
    char buffer[STRING_SIZE];

    char_to_string(False, 0x0309, buffer, False);
    ASSERT_STR_EQ("◌̉", buffer);

    PASS();
}

TEST char_to_string_converts_astral_combiner() {
    char buffer[STRING_SIZE];

    char_to_string(False, 0x0101FD, buffer, False);
    ASSERT_STR_EQ("◌𐇽", buffer);

    PASS();
}

SUITE(cstring_to_text_suite) {
    SET_SETUP(initialize_texts, (Text*[]) {text, NULL});
    SET_TEARDOWN(deinitialize_texts, (Text*[]) {text, NULL});

    RUN_TEST(cstring_to_text_should_handle_ascii_strings);
    RUN_TEST(cstring_to_text_should_handle_latin);
    RUN_TEST(cstring_to_text_should_handle_bmp);
    RUN_TEST(cstring_to_text_should_handle_astral_code_points);
}

SUITE(char_to_string_suite) {
    RUN_TEST(char_to_string_converts_printable_ascii);
    RUN_TEST(char_to_string_converts_space);
    RUN_TEST(char_to_string_converts_non_printable_ascii);
    RUN_TEST(char_to_string_converts_printable_bmp);
    RUN_TEST(char_to_string_converts_astral_code_points);
    RUN_TEST(char_to_string_converts_bmp_combiner);
    RUN_TEST(char_to_string_converts_astral_combiner);
}
