# Get LOCAL_INCLUDE_DIR
include ../common.mk

CPPFLAGS += -MMD

LIB = libisri.a
SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))
DEPS := $(patsubst %.c,%.d,$(SRCS))

# Standard targets.
all: $(LIB)

$(LIB): $(OBJS)
	$(AR) $(ARFLAGS) -s $@ $^

# Include dependencies from -d
-include $(DEPS)

clean:
	-$(RM) $(LIB)
	-$(RM) *.o
	-$(RM) *.d

.PHONY: all clean
