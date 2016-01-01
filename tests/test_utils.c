#include "test_utils.h"

#include <list.h>

static Text text_;
Text* text = &text_;

void initialize_texts(void *list) {
    Text** texts = (Text**) list;
    for (; *texts != NULL; texts++) {
        list_initialize(*texts);
    }
}

void deinitialize_texts(void *list) {
    Text** texts = (Text**) list;
    for (; *texts != NULL; texts++) {
        /* Frees each character and clears the text. */
        list_empty(*texts, free);
    }
}
