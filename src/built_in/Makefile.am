lib_LIBRARIES = libbuilt_in.a libdbuilt_in.a

libbuilt_in_a_SOURCES =  \
    echo.c \
    true_false.c \
    cd.c \
    exit.c \
    built_in.h

libbuilt_in_a_CPPFLAGS = -I$(top_srcdir)/src

libbuilt_in_a_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

libdbuilt_in_a_SOURCES =  \
    echo.c \
    true_false.c \
    cd.c \
    exit.c \
    built_in.h

libdbuilt_in_a_CPPFLAGS = -I$(top_srcdir)/src

libdbuilt_in_a_CFLAGS = -g -O0 -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

libdbuilt_in_a_LFLAGS = -fsanitize=address

noinst_LIBRARIES = libbuilt_in.a libdbuilt_in.a
