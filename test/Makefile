include ../use-libisri-internal.mk

test: unit-test large-file-test accsum-test

unit-test: run
	./$< $(TEST_ARGS)

large-file-test:
	./test_accuracy_large_files.py

accsum-test:
	./test_accsum_graphic_characters.py

clean:
	$(RM) run

run: run.c $(wildcard *_test.c) $(LIBISRI) test_utils.c test_utils.h
	$(LINK.c) test_utils.c $< -lisri -lutf8proc -o $@

.PHONY: test clean accsum-test large-file-test unit-test
