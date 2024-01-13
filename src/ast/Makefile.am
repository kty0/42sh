lib_LIBRARIES = libast.a libdast.a

libast_a_SOURCES =  \
	ast.c \
	ast.h

libast_a_CPPFLAGS = -I$(top_srcdir)/src

libast_a_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

libdast_a_SOURCES =  \
	ast.c \
	ast.h

libdast_a_CPPFLAGS = -I$(top_srcdir)/src

libdast_a_CFLAGS = -g -Og -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

noinst_LIBRARIES = libast.a libdast.a
