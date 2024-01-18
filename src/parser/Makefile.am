lib_LIBRARIES = libparser.a libdparser.a

libparser_a_SOURCES =  \
	parser.c \
	parser.h

libparser_a_CPPFLAGS = -I$(top_srcdir)/src

libparser_a_CFLAGS = -g -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

libdparser_a_SOURCES =  \
	parser.c \
	parser.h

libdparser_a_CPPFLAGS = -I$(top_srcdir)/src

libdparser_a_CFLAGS = -g -Og -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

libdparser_a_LDFLAGS = -fsanitize=address

noinst_LIBRARIES = libparser.a libdparser.a
