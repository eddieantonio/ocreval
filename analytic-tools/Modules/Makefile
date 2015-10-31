# Get LOCAL_INCLUDE_DIR
include ../common.mk

CPPFLAGS += -MMD

LIB = Lib.a
SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))
DEPS := $(patsubst %.c,%.d,$(SRCS))

# Standard targets.
all: $(LIB)

$(LIB): $(OBJS)
	ar -rs $@ $(OBJS)

# Include dependencies from -d
-include $(DEPS)

clean:
	-$(RM) $(LIB)
	-$(RM) *.o
	-$(RM) *.d

.PHONY: all clean
