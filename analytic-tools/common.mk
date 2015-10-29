# Get absolute path to containing directory: http://stackoverflow.com/a/324782
TOP := $(dir $(CURDIR)/$(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST)))
LOCAL_INCLUDE_DIR := $(TOP)Modules

CFLAGS = -ansi
CPPFLAGS = -MMD -I$(LOCAL_INCLUDE_DIR)
