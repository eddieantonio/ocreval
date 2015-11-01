# Get absolute path to containing directory: http://stackoverflow.com/a/324782
TOP := $(dir $(CURDIR)/$(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST)))
LOCAL_INCLUDE_DIR := $(abspath $(TOP)Modules)

include $(TOP)/common.mk

LDFLAGS = $(LOCAL_INCLUDE_DIR)/Lib.a -lm

# ld(1) needs the LDFLAGS at the end for some dumb reason.
%: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $< $(LDFLAGS)
