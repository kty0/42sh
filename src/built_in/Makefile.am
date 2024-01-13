lib_LIBRARIES = libbuilt_in.a libdbuilt_in.a

libbuilt_in_a_SOURCES =  \
	echo.c \
	echo.h \
	true_false.c \
	true_false.h

libbuilt_in_a_CPPFLAGS = -I$(top_srcdir)/src

libbuilt_in_a_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

libdbuilt_in_a_SOURCES =  \
	echo.c \
	echo.h \
	true_false.c \
	true_false.h

libdbuilt_in_a_CPPFLAGS = -I$(top_srcdir)/src

libdbuilt_in_a_CFLAGS = -g -Og -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

noinst_LIBRARIES = libbuilt_in.a libdbuilt_in.a
