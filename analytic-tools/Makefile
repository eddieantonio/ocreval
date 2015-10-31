include common.mk

TOOLS = accci accdist accsum accuracy asc2uni editop editopcost \
	editopsum groupacc ngram nonstopacc synctext uni2asc vote wordacc \
	wordaccci wordaccdist wordaccsum wordfreq

DIRS = $(TOOLS:=/)
EXECUTABLES = $(foreach D,$(TOOLS),$D/$D)
MANPAGES = $(EXECUTABLES:=.1)
LIB = Modules/Lib.a

all: $(DIRS)

# Create every subexecutable.
%/: % $(LIB)
	$(MAKE) -C $<

$(LIB):
	$(MAKE) -C $(dir $@)

clean: clean-lib clean-execs

clean-lib:
	$(MAKE) -C Modules clean

clean-execs:
	$(RM) $(EXECUTABLES)

# Make Lib.a a phony rule to
.PHONY: all clean clean-lib clean-execs
