check_PROGRAMS = \
	testlexer

testlexer_SOURCES = testlexer.c

testlexer_CFLAGS = -std=c99 -Wall -Wextra -Wvla -pedantic

testlexer_LDFLAGS = -fsanitize=address

testlexer_CPPFLAGS = -I$(top_srcdir)/src

testlexer_LDADD = \
	-lcriterion \
	../src/lexer/liblexer.a

#	./forbidden.sh
check-local:
	-./testsuite.sh
	-./testlexer

check-lexer:
	./testlexer
