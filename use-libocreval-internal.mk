# Get absolute path to containing directory: http://stackoverflow.com/a/324782
TOP := $(dir $(CURDIR)/$(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST)))
# For INTERNAL headers!
LOCAL_INCLUDE_DIR := $(abspath $(TOP)src)
# For use with the -L option.
LOCAL_LINK_DIR := $(abspath $(TOP)lib)

LIBOCREVAL = $(LOCAL_LINK_DIR)/libocreval.a

# Compilation flags for all files.
override CFLAGS += -ansi
# X/Open 6.0 standardizes features used in this K&R C source...
CPPDEFINES = -D_XOPEN_SOURCE=600
# Create dependency files.
CPPFLAGS = -MMD
# utf8proc lib usually lives in here:
override CPPFLAGS += -I/usr/local/include $(CPPDEFINES)
LDFLAGS += -L/usr/local/lib
LDLIBS = -lm -lutf8proc

# Use libocreval, created in lib/
override CPPFLAGS += -I$(LOCAL_INCLUDE_DIR)
LDFLAGS += -L$(LOCAL_LINK_DIR)
LDLIBS := -locreval $(LDLIBS)
