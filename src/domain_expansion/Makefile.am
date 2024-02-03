lib_LIBRARIES = libdomainexpansion.a libddomainexpansion.a

libdomainexpansion_a_SOURCES =  \
	domain_expansion.c \
	domain_expansion.h \
	string_builder.c \
	string_builder.h

libdomainexpansion_a_CPPFLAGS = -I$(top_srcdir)/src

libdomainexpansion_a_CFLAGS = -g -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

libddomainexpansion_a_SOURCES =  \
	domain_expansion.c \
	domain_expansion.h \
	string_builder.c \
	string_builder.h

libddomainexpansion_a_CPPFLAGS = -I$(top_srcdir)/src

libddomainexpansion_a_CFLAGS = -g -O0 -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

libddomainexpansion_a_LFLAGS = -fsanitize=address

noinst_LIBRARIES = libdomainexpansion.a libddomainexpansion.a

