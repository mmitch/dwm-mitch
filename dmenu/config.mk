# dmenu version
VERSION = 4.4.1

# dwm-mitch patchlevel
VERSION := ${VERSION}+${PATCHVERSION}

# paths
PREFIX = /usr/local
MANPREFIX = ${PREFIX}/share/man

X11INC = /usr/X11R6/include
X11LIB = /usr/X11R6/lib

# Xinerama, comment if you don't want it
XINERAMALIBS  = -lXinerama
XINERAMAFLAGS = -DXINERAMA

# includes and libs
INCS = -I${X11INC}
LIBS = -L${X11LIB} -lX11 ${XINERAMALIBS}

# no -flto with clang
ifeq (${CC},clang)
	FLTO =
else
	FLTO = -flto
endif

# flags
CPPFLAGS = -D_BSD_SOURCE -DVERSION=\"${VERSION}\" ${XINERAMAFLAGS}
CFLAGS   = $(FLTO) -ansi -pedantic -Wall -Os ${INCS} ${CPPFLAGS}
LDFLAGS  = $(FLTO) -Os -s ${LIBS}

# compiler and linker
CC ?= cc
