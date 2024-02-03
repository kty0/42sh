lib_LIBRARIES = libeval.a libdeval.a

libeval_a_SOURCES =  \
	eval.c \
	eval.h \
	eval_redir.c

libeval_a_CPPFLAGS = -I$(top_srcdir)/src

libeval_a_CFLAGS = -g -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

libdeval_a_SOURCES =  \
	eval.c \
	eval.h \
	eval_redir.c

libdeval_a_CPPFLAGS = -I$(top_srcdir)/src

libdeval_a_CFLAGS = -g -O0 -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

libdeval_a_LFLAGS = -fsanitize=address

noinst_LIBRARIES = libeval.a libdeval.a

