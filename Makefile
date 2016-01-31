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

# Install prefix, if needed.
PREFIX = /usr/local

# List of all the tools (executables + manual pages)
TOOLS = accci accdist accsum accuracy editop editopcost editopsum \
		groupacc ngram nonstopacc synctext vote wordacc wordaccci \
		wordaccdist wordaccsum wordfreq

# Name: libisri, or -lisri
NAME = isri
MAJOR_VERSION = 6
MINOR_VERSION = 1

# All the executables go in bin/
EXECUTABLES = $(addprefix bin/,$(TOOLS))
# All manual pages go in share/man/man1
MANPAGES = $(foreach TOOL,$(TOOLS),share/man/man1/$(TOOL))

include use-libisri-internal.mk

LIBRARY.a = lib/lib$(NAME).a
ifeq ($(shell uname),Darwin)
LIBRARY.so = $(LIBRARY.a:.a=.dylib)
else
LIBRARY.so = $(LIBRARY.a:.a=.so.$(MAJOR_VERSION).$(MINOR_VERSION))
endif


################################################################################

# Allows for proper compilation and linking settings for libisri

all: $(EXECUTABLES)

install: install-bin install-man

install-bin: $(TOOLS)
	mkdir -p $(BINDIR)
	cp $(EXECUTABLES) $(BINDIR)/

install-man: $(TOOLS)
	mkdir -p $(MANDIR)
	cp $(MANPAGES) $(MANDIR)/

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
test: $(LIBRARY.a)
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
# Modules are all object files exclusively for inclusion in libisri
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

# Special case: Generate this include file, required by libisri.a
src/word_break_property.h: libexec/generate_word_break.py libexec/WordBreakProperty.txt.gz
	./$< > $@
