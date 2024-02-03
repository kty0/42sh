lib_LIBRARIES = libparser.a libdparser.a

libparser_a_SOURCES =  \
	parser.c \
	parser.h \
	parser_io.c \
	parser_io.h \
	parser_if.c \
	parser_if.h \
	parser_command.c \
	parser_command.h \
	parser_misc.c \
	parser_misc.h \
	parser_loop.c \
	parser_loop.h

libparser_a_CPPFLAGS = -I$(top_srcdir)/src

libparser_a_CFLAGS = -g -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

libdparser_a_SOURCES =  \
	parser.c \
	parser.h \
	parser_io.c \
	parser_io.h \
	parser_if.c \
	parser_if.h \
	parser_command.c \
	parser_command.h \
	parser_misc.c \
	parser_misc.h \
	parser_loop.c \
	parser_loop.h

libdparser_a_CPPFLAGS = -I$(top_srcdir)/src

libdparser_a_CFLAGS = -g -O0 -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

libdparser_a_LFLAGS = -fsanitize=address

noinst_LIBRARIES = libparser.a libdparser.a
