# Compilation flags for all files.
CFLAGS += -ansi
# utf8proc lib usually lives in here:
CPPFLAGS += -I/usr/local/include
LDFLAGS += -L/usr/local/lib
LDLIBS = -lm -lutf8proc
