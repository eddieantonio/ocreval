# Copyright 2016 Eddie Antonio Santos <easantos@ualberta.ca>
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# Install prefix, if installing globally.
# See also: `exports` target
PREFIX = /usr/local
BINDIR = $(PREFIX)/bin
MANDIR = $(PREFIX)/share/man/man1

# List of all the tools (executables + manual pages)
TOOLS = accci accdist accsum accuracy editop editopcost editopsum \
		groupacc ngram nonstopacc synctext vote wordacc wordaccci \
		wordaccdist wordaccsum wordfreq

# Name: libocreval, or -locreval
NAME = ocreval
MAJOR_VERSION = 7
MINOR_VERSION = 0

# All the executables go in bin/
EXECUTABLES = $(addprefix bin/,$(TOOLS))
# All manual pages go in share/man/man1
MANPAGES = $(foreach TOOL,$(TOOLS),share/man/man1/$(TOOL).1)

include use-libocreval-internal.mk

LIBRARY.a = lib/lib$(NAME).a
ifeq ($(shell uname -s),Darwin)
LIBRARY.so = $(LIBRARY.a:.a=.dylib)
else
LIBRARY.so = $(LIBRARY.a:.a=.so.$(MAJOR_VERSION).$(MINOR_VERSION))
endif

################################################################################

# Allows for proper compilation and linking settings for libocreval

all: $(EXECUTABLES)

install: install-bin install-man

install-bin: $(EXECUTABLES)
	mkdir -p $(BINDIR)
	cp $(EXECUTABLES) $(BINDIR)/

install-man: $(MANPAGES)
	mkdir -p $(MANDIR)
	cp $(MANPAGES) $(MANDIR)/

# Prints a bunch of exports you can source in your shell's startup file.
exports:
	@echo '#' ocreval
	@echo export PATH='$(TOP)bin:$$PATH'
	@echo export MANPATH='$(TOP)share/man:$$MANPATH'
ifeq ($(shell uname -s),Darwin)
	@echo export DYLD_LIBRARY_PATH='$(TOP)lib:$$DYLD_LIBRARY_PATH'
else
	@echo export LD_LIBRARY_PATH='$(TOP)lib:$$LD_LIBRARY_PATH'
endif

clean: clean-objs clean-execs clean-libs clean-deps clean-test

clean-libs:
	$(RM) $(LIBRARY.a) $(LIBRARY.so)

clean-objs:
	$(RM) $(MODULES:.c=.o)

clean-deps:
	$(RM) $(DEPENDENCIES)

clean-execs:
	$(RM) $(EXECUTABLES)

clean-test:
	$(MAKE) -C test clean

TEST_ARGS =
test: $(EXECUTABLES) $(LIBRARY.a)
	$(MAKE) -C test

# Uses https://github.com/alexch/rerun
# $ gem install rerun
watch:
	rerun --clear --exit --pattern '**/*.{c,h}' -- make test

.PHONY: all
.PHONY: install install-bin install-man
.PHONY: clean clean-deps clean-execs clean-lib clean-objs clean-test
.PHONY: test watch
################################################################################

# Executable sources are C files that provide a main() for executables.
EXECUTABLE_SOURCES := $(foreach TOOL,$(TOOLS),src/$(TOOL).c)
# Modules are all object files exclusively for inclusion in libocreval
MODULE_SOURCES := $(filter-out $(EXECUTABLE_SOURCES),$(wildcard src/*.c))
MODULES := $(MODULE_SOURCES:.c=.o)
# Dependencies are .d files included by Make
DEPENDENCIES := $(EXECUTABLES:=.d) $(MODULES:.o=.d)

-include $(DEPENDENCIES)

# Rules for building executables; they are statically linked with the library.
bin/%: src/%.c $(LIBRARY.a)
	$(LINK.c) -o $@ $< $(LDLIBS)

$(LIBRARY.a): $(MODULES)
	$(AR) $(ARFLAGS) -s $@ $^

# Special case: Generate this include file, required by libocreval.a
$(TOP)src/word_break_property.h src/word_break_property.h: \
		libexec/generate_word_break.py libexec/WordBreakProperty.txt.gz
	./$< > $@
