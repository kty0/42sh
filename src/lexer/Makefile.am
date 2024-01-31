lib_LIBRARIES = liblexer.a libdlexer.a

liblexer_a_SOURCES =  \
	lexer.c \
	lexer.h \
	get_string.c

liblexer_a_CPPFLAGS = -I$(top_srcdir)/src

liblexer_a_CFLAGS = -g -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

libdlexer_a_SOURCES =  \
	lexer.c \
	lexer.h \
	get_string.c

libdlexer_a_CPPFLAGS = -I$(top_srcdir)/src

libdlexer_a_CFLAGS = -g -Og -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

libdlexer_a_LFLAGS = -fsanitize=address

noinst_LIBRARIES = liblexer.a libdlexer.a
