# define the sub directories
SUBDIRS = ast \
	parser \
	lexer \
	built_in \
	eval

bin_PROGRAMS = 42sh 42debug

42sh_SOURCES = 42sh.c

42sh_CPPFLAGS = -I%D%

42sh_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

42sh_LDADD =  \
	parser/libparser.a \
	ast/libast.a \
	lexer/liblexer.a \
	eval/libeval.a \
	built_in/libbuilt_in.a

42debug_SOURCES = 42sh.c

42debug_CPPFLAGS = -I%D%

42debug_CFLAGS = -g -Og -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

42debug_LDADD =  \
	parser/libdparser.a \
	ast/libdast.a \
	lexer/libdlexer.a \
	eval/libdeval.a \
	built_in/libdbuilt_in.a
