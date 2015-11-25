include common.mk

TOOLS = accci accdist accsum accuracy editop editopcost editopsum \
		groupacc ngram nonstopacc synctext vote wordacc wordaccci \
		wordaccdist wordaccsum wordfreq

PREFIX = /usr/local
BINDIR = $(PREFIX)/bin
MANDIR = $(PREFIX)/share/man/man1

DIRS = $(TOOLS:=/)
EXECUTABLES = $(foreach D,$(TOOLS),$D/$D)
MANPAGES = $(EXECUTABLES:=.1)
LIB = Modules/libisri.a

all: $(DIRS)

# Create every subexecutable.
%/: $(LIB)
	$(MAKE) -C $@

$(LIB):
	$(MAKE) -C $(dir $@)

install: install-bin install-man

install-bin: $(DIRS)
	mkdir -p $(BINDIR)
	cp $(EXECUTABLES) $(BINDIR)/

install-man: $(DIRS)
	mkdir -p $(MANDIR)
	cp $(MANPAGES) $(MANDIR)/

clean: clean-lib clean-execs

clean-lib:
	$(MAKE) -C Modules clean

clean-execs:
	$(RM) $(EXECUTABLES)

# Make Lib.a a phony rule to
.PHONY: all clean clean-lib clean-execs install install-bin install-bin
