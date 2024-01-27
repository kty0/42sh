check_PROGRAMS = \
	testlexer

testlexer_SOURCES = criterion-tests/testlexer.c

testlexer_CFLAGS = -std=c99 -Wall -Wextra -Wvla -pedantic

testlexer_CPPFLAGS = -I$(top_srcdir)/src

testlexer_LDADD = \
	-lcriterion \
	../src/lexer/liblexer.a

check-local:
	./forbidden.sh
	-./testlexer
	-./testsuite.sh -h

check-lexer:
	./testlexer
