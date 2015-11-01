# Get absolute path to containing directory: http://stackoverflow.com/a/324782
TOP := $(dir $(CURDIR)/$(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST)))
LOCAL_INCLUDE_DIR := $(abspath $(TOP)Modules)

include $(TOP)/common.mk

# Add libisri, created in Modules/
LDFLAGS = -L$(LOCAL_INCLUDE_DIR)
LDLIBS = -lisri -lm
