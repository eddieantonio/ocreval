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

ifeq ($(shell uname),Darwin)
LIBRARY.so = $(LIBRARY.a:.a=.dylib)
else
LIBRARY.so = $(LIBRARY.a:.a=.so.$(MAJOR_VERSION).$(MINOR_VERSION))
endif

# Allows for proper compilation and linking settings for libisri

all: $(EXECUTABLES)

###

# Rules for building executables; they are statically linked with the library.
bin/%: src/%.c $(LIBRARY.a)
	$(LINK.c) -o $@ $< $(LDLIBS)

EXECUTABLE_SOURCES := $(foreach TOOL,$(TOOLS),src/$(TOOL).c)
MODULES := $(filter-out $(EXECUTABLE_SOURCES),$(wildcard src/*.c))

$(LIBRARY.a): $(MODULES:.c=.o)
	$(AR) $(ARFLAGS) -s $@ $^

.PHONY: all
