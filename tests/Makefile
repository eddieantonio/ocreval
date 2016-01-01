include ../use_lib.mk

test: run
	./$< $(TEST_ARGS)

run: run.c $(wildcard *_test.c) $(LIBISRI) test_utils.c test_utils.h
	$(LINK.c) test_utils.c $< -lisri -lutf8proc -o $@

.PHONY: test
