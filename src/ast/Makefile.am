lib_LIBRARIES = libast.a libdast.a

libast_a_SOURCES =  \
	ast.c \
	ast.h \
	ast_new.c \
	ast_new.h \
	ast_free.c

libast_a_CPPFLAGS = -I$(top_srcdir)/src

libast_a_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

libdast_a_SOURCES =  \
	ast.c \
	ast.h \
	ast_new.c \
	ast_new.h \
	ast_free.c

libdast_a_CPPFLAGS = -I$(top_srcdir)/src

libdast_a_CFLAGS = -g -O0 -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

libdast_a_LFLAGS = -fsanitize=address

noinst_LIBRARIES = libast.a libdast.a
