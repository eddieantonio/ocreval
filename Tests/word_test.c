#include "greatest.h"
#include "test_utils.h"

#include <word.h>

static Wordlist wordlist_;
static Wordlist *wordlist = &wordlist_;

/* Aliases for traversing the linked list. */
/* ALWAYS ensure sufficient length before using these aliases. */
#define second  first->next
#define third   first->next->next
#define fourth  first->next->next->next
#define fifth   first->next->next->next->next
#define sixth   first->next->next->next->next->next
#define seventh first->next->next->next->next->next->next
#define eighth  first->next->next->next->next->next->next->next
#define nineth  first->next->next->next->next->next->next->next->next

/* Exercises ASCII characters. */
TEST find_words_segments_english_words() {
    /* From: http://unicode.org/reports/tr29/#Word_Boundaries */
    cstring_to_text(text, "The quick (\"brown\") fox can’t jump 32.3 feet, "
                          "right?");
    find_words(wordlist, text);

    ASSERT_EQ_FMT(9, wordlist->count, "%d");
    ASSERT_STR_EQ("quick", wordlist->second->string);
    ASSERT_STR_EQ("brown", wordlist->third->string);
    ASSERT_STR_EQ("fox",   wordlist->fourth->string);
    ASSERT_STR_EQ("can't", wordlist->fifth->string);
    ASSERT_STR_EQ("jump",  wordlist->sixth->string);
    ASSERT_STR_EQ("32.3",  wordlist->seventh->string);
    ASSERT_STR_EQ("right",  wordlist->nineth->string);

    PASS();
}

/* Exercises Latin-1 characters and punctuation. */
TEST find_words_segments_spanish_words() {
    cstring_to_text(text, "¡Feliz año nuevo!");
    find_words(wordlist, text);

    ASSERT_EQ_FMT(3, wordlist->count, "%d");
    ASSERT_STR_EQ("Feliz", wordlist->first->string);
    ASSERT_STR_EQ("año", wordlist->second->string);
    ASSERT_STR_EQ("nuevo", wordlist->third->string);

    PASS();
}

/* Exercises numeric processing. */
TEST find_words_segments_numerals() {
    /* From https://github.com/eddieantonio/isri-ocr-evaluation-tools/issues/3 */
    cstring_to_text(text, "PLASTIK-KARTON BARDA %18 *1,75");
    find_words(wordlist, text);

    ASSERT_EQ_FMT(5, wordlist->count, "%d");
    ASSERT_STR_EQ("PLASTIK", wordlist->first->string);
    ASSERT_STR_EQ("KARTON", wordlist->second->string);
    ASSERT_STR_EQ("BARDA", wordlist->third->string);
    ASSERT_STR_EQ("18", wordlist->fourth->string);
    ASSERT_STR_EQ("1,75", wordlist->fifth->string);

    PASS();
}

/* Exercises numeric processing. */
TEST find_words_segments_japanese() {
    /* This phrase -- rōkaraizu no densetsu 'Legend of Localization' -- is
     * conveniently written in katakana, hiragana, and kanji, respectively. */
    cstring_to_text(text, "ローカライズの伝説");
    find_words(wordlist, text);

    ASSERT_EQ_FMT(3, wordlist->count, "%d");
    ASSERT_STR_EQ("ローカライズ", wordlist->first->string);
    ASSERT_STR_EQ("の", wordlist->second->string);
    ASSERT_STR_EQ("伝説", wordlist->third->string);

    PASS();
}

#undef second
#undef third
#undef fourth
#undef fifth

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

    RUN_TEST(find_words_segments_english_words);
    RUN_TEST(find_words_segments_spanish_words);
    RUN_TEST(find_words_segments_numerals);
    RUN_TEST(find_words_segments_japanese);
}
