include ../use_lib.mk

test: run
	./$< $(TEST_ARGS)

run: run.c $(wildcard *_test.c) $(LIBISRI)
	$(LINK.c) $< -lisri -lutf8proc -o $@

.PHONY: test
