lib_LIBRARIES = libhashmap.a libdhashmap.a

libhashmap_a_SOURCES =  \
	hash_map.c \
	hash_map.h \
	hash.c

libhashmap_a_CPPFLAGS = -I$(top_srcdir)/src

libhashmap_a_CFLAGS = -g -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

libdhashmap_a_SOURCES =  \
	hash_map.c \
	hash_map.h \
	hash.c

libdhashmap_a_CPPFLAGS = -I$(top_srcdir)/src

libdhashmap_a_CFLAGS = -g -Og -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

libdhashmap_a_LFLAGS = -fsanitize=address

noinst_LIBRARIES = libhashmap.a libdhashmap.a

