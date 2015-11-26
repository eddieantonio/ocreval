# Compilation flags for all files.
CFLAGS += -ansi
# This code was originally written in the mid-90s;
# X/Open 5.0 was all there was :/
CPPDEFINES = -D_XOPEN_SOURCE=500
# utf8proc lib usually lives in here:
CPPFLAGS := $(CPPFLAGS) -I/usr/local/include $(CPPDEFINES)
LDFLAGS += -L/usr/local/lib
LDLIBS = -lm -lutf8proc
