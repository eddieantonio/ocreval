# Compilation flags for all files.
CFLAGS += -ansi
# X/Open 6.0 standardizes features used in this K&R C source...
CPPDEFINES = -D_XOPEN_SOURCE=600
# utf8proc lib usually lives in here:
CPPFLAGS := $(CPPFLAGS) -I/usr/local/include $(CPPDEFINES)
LDFLAGS += -L/usr/local/lib
LDLIBS = -lm -lutf8proc
