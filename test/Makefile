include ../use-libocreval-internal.mk

test: unit-test large-file-test accsum-test

unit-test: run
	./$< $(TEST_ARGS)

large-file-test:
	./test_accuracy_large_files.py

accsum-test:
	./test_accsum_graphic_characters.py

clean:
	$(RM) run

run: run.c $(wildcard *_test.c) $(LIBOCREVAL) test_utils.c test_utils.h
	$(LINK.c) test_utils.c $< -locreval -lutf8proc -o $@

.PHONY: test clean accsum-test large-file-test unit-test
