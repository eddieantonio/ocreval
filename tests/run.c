#include "wordacc_test.c"

#include "greatest.h"

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
    GREATEST_MAIN_BEGIN();

    RUN_SUITE(wordacc_suite);

    GREATEST_MAIN_END();
}
