# dwm version
VERSION = 4.7

# Customize below to fit your system

# dwm-mitch patchlevel
VERSION := $(VERSION)+$(PATCHVERSION)

# paths
PREFIX = /usr/local
MANPREFIX = $(PREFIX)/share/man

X11INC = /usr/X11R6/include
X11LIB = /usr/X11R6/lib

# includes and libs
INCS = -I. -I/usr/include -I$(X11INC)
LIBS = -L/usr/lib -lc -L$(X11LIB) -lX11 -lXinerama

# no -flto with clang
ifeq ($(CC),clang)
	FLTO =
else
	FLTO = -flto
endif

# flags
CFLAGS = $(FLTO) -Os $(INCS) -DVERSION=\"$(VERSION)\"
LDFLAGS = $(FLTO) -Os -s $(LIBS)
#CFLAGS = -g -std=c99 -pedantic -Wall -O2 $(INCS) -DVERSION=\"$(VERSION)\"
#LDFLAGS = -g $(LIBS)

# Solaris
#CFLAGS = -fast $(INCS) -DVERSION=\"$(VERSION)\"
#LDFLAGS = $(LIBS)
#CFLAGS += -xtarget=ultra

# compiler and linker
CC ?= cc
