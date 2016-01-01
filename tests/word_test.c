#include "greatest.h"
#include "test_utils.h"

#include <word.h>

static Wordlist wordlist_;
static Wordlist *wordlist = &wordlist_;

/* Aliases for traversing the linked list. */
/* ALWAYS ensure sufficient length before using this aliases. */
#define second  first->next
#define third   first->next->next

/* Exercises ASCII characters. */
TEST find_words_works_for_english() {
    cstring_to_text(text, "Hello, world!");
    find_words(wordlist, text);

    ASSERT_EQ_FMT(2, wordlist->count, "%d");
    ASSERT_STR_EQ("Hello", wordlist->first->string);
    ASSERT_STR_EQ("world", wordlist->second->string);

    PASS();
}

/* Exercises Latin-1 characters and punctuation. */
TEST find_words_works_for_spanish() {
    cstring_to_text(text, "¡Feliz año nuevo!");
    find_words(wordlist, text);

    ASSERT_EQ_FMT(3, wordlist->count, "%d");
    ASSERT_STR_EQ("Feliz", wordlist->first->string);
    ASSERT_STR_EQ("año", wordlist->second->string);
    ASSERT_STR_EQ("nuevo", wordlist->third->string);

    PASS();
}

#undef second
#undef third

static void setup_find_words(void *unused) {
    initialize_texts((Text*[]) {text, NULL});
    list_initialize(wordlist);
}

static void teardown_find_words(void *unused) {
    deinitialize_texts((Text*[]) {text, NULL});
    list_empty(wordlist, free_word);
}

SUITE(find_words_suite) {
    SET_SETUP(setup_find_words, NULL);
    SET_TEARDOWN(teardown_find_words, NULL);

    RUN_TEST(find_words_works_for_english);
    RUN_TEST(find_words_works_for_spanish);
}
