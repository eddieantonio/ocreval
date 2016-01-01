#include "text_test.c"
#include "word_test.c"

#include "greatest.h"

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
    GREATEST_MAIN_BEGIN();

    RUN_SUITE(cstring_to_text_suite);
    RUN_SUITE(char_to_string_suite);

    RUN_SUITE(find_words_suite);

    GREATEST_MAIN_END();
}
