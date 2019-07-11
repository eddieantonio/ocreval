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
#define ninth   first->next->next->next->next->next->next->next->next


TEST find_words_segments_a_single_ascii_word() {
    cstring_to_text(text, "C11");
    find_words(wordlist, text);

    ASSERT_EQ_FMT(1, wordlist->count, "%d");
    ASSERT_STR_EQ("C11", wordlist->first->string);
    PASS();
}

TEST find_words_returns_nfc() {
    char pho_nfc[] = { 'p', 'h', 0xE1, 0xBB, 0x9F, 0 };
    /* With two combining characters. */
    cstring_to_text(text, (char []) { 'p', 'h', 'o',
                                      0xCC, 0x9B, /* ◌̛ */
                                      0xCC, 0x89, /* ◌̉ */
                                      0 });
    find_words(wordlist, text);

    ASSERT_EQ_FMT(1, wordlist->count, "%d");
    ASSERT_STR_EQ(pho_nfc, wordlist->first->string);
    PASS();
}

TEST find_words_returns_zero_when_not_given_words() {
    /* With two combining characters. */
    cstring_to_text(text, "#$#@! #@!\n#@!!!$#");
    find_words(wordlist, text);

    ASSERT_EQ_FMT(0, wordlist->count, "%d");
    PASS();
}


/* Exercises ASCII characters. */
TEST find_words_segments_english_with_punctuation() {
    /* From: http://unicode.org/reports/tr29/#Word_Boundaries */
    cstring_to_text(text, "The quick (\"brown\") fox can’t jump 32.3 feet, "
                          "right?");
    find_words(wordlist, text);

    ASSERT_EQ_FMT(9, wordlist->count, "%d");
    ASSERT_STR_EQ("quick", wordlist->second->string);
    ASSERT_STR_EQ("brown", wordlist->third->string);
    ASSERT_STR_EQ("fox",   wordlist->fourth->string);
    ASSERT_STR_EQ("can’t", wordlist->fifth->string);
    ASSERT_STR_EQ("jump",  wordlist->sixth->string);
    ASSERT_STR_EQ("32.3",  wordlist->seventh->string);
    ASSERT_STR_EQ("right",  wordlist->ninth->string);

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
    /* From https://github.com/eddieantonio/ocreval/issues/3 */
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

    ASSERT_EQ_FMT(4, wordlist->count, "%d");
    ASSERT_STR_EQ("ローカライズ", wordlist->first->string);
    ASSERT_STR_EQ("の", wordlist->second->string);
    /* A Japanese- tailored algorithm would segment this into three words
     * instead of four, however, that would involve incorporating a Japanese
     * dictionary in order to look-up Kanji words... */
    ASSERT_STR_EQ("伝", wordlist->third->string);
    ASSERT_STR_EQ("説", wordlist->fourth->string);

    PASS();
}

TEST find_words_segments_haida_words() {
    cstring_to_text(text, "Wᴀˊstᴀ haˊoîsîn ᵋāl ʟēˊłas ʟ̣ū haoîsîˊn\n"
                          "l’ sᵋaiˊᵋänᴀn.");
    find_words(wordlist, text);

    ASSERT_EQ_FMT(8, wordlist->count, "%d");
    ASSERT_STR_EQ("Wᴀˊstᴀ",     wordlist->first->string);
    ASSERT_STR_EQ("haˊoîsîn",   wordlist->second->string);
    ASSERT_STR_EQ("ᵋāl",        wordlist->third->string);
    ASSERT_STR_EQ("ʟēˊłas",     wordlist->fourth->string);
    ASSERT_STR_EQ("ʟ̣ū",         wordlist->fifth->string);
    ASSERT_STR_EQ("haoîsîˊn",   wordlist->sixth->string);
    ASSERT_STR_EQ("sᵋaiˊᵋänᴀn", wordlist->eighth->string);

    PASS();
}

/* Regression test -- word boundaries segfaults on control characters.
 * See: https://github.com/eddieantonio/ocreval/issues/22#issuecomment-491448129 */
TEST find_words_control_character() {
    cstring_to_text(text, "\003");
    find_words(wordlist, text);

    ASSERT_EQ_FMT(0, wordlist->count, "%d");

    PASS();
}


#undef second
#undef third
#undef fourth
#undef fifth
#undef sixth
#undef seventh
#undef eighth
#undef ninth

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

    RUN_TEST(find_words_segments_a_single_ascii_word);
    RUN_TEST(find_words_returns_nfc);
    RUN_TEST(find_words_returns_zero_when_not_given_words);
    RUN_TEST(find_words_segments_english_with_punctuation);
    RUN_TEST(find_words_segments_spanish_words);
    RUN_TEST(find_words_segments_haida_words);
    RUN_TEST(find_words_segments_numerals);
    RUN_TEST(find_words_segments_japanese);
    RUN_TEST(find_words_control_character);
}
