#!/bin/sh

REF_OUT="/tmp/.bash.out"
TEST_OUT="/tmp/.42sh.out"
DIFF_OUT="/tmp/.diff.out"
ERR_REF_OUT="/tmp/.err.out"
ERR_TEST_OUT="/tmp/.err_test.out"

RED='\033[0;31m'
GREEN='\033[0;32m'
GRAY='\033[0;37m'
NC='\033[0m'

dir_scripts="scripts"
dir_arguments="arguments"

my42sh="../src/./42sh"

testcase_as_arg()
{
    bash --posix -c "$1" > "$REF_OUT" 2>"$ERR_REF_OUT"
    exit_code_ref=$?
    $my42sh -c "$1" > "$TEST_OUT" 2> "$ERR_TEST_OUT"
    exit_code_test=$?
    diff -u "$REF_OUT" "$TEST_OUT" > "$DIFF_OUT"
    if [ $? -eq 1 ] || [ $exit_code_ref -ne $exit_code_test ]; then
        echo -e "${RED}[T-T]${NC} $1"
        cat "$DIFF_OUT"
    else
        echo -e "${GREEN}[UwU]${NC} $1"
    fi
    echo "  ________________________________________________________________"
}

testcase_as_script()
{
    bash --posix "$1" > "$REF_OUT" 2>"$ERR_REF_OUT"
    exit_code_ref=$?
    $my42sh $1 > "$TEST_OUT" 2> "$ERR_TEST_OUT"
    exit_code_test=$?
    diff -u "$REF_OUT" "$TEST_OUT" > "$DIFF_OUT"
    if [ $? -eq 1 ] || [ $exit_code_ref -ne $exit_code_test ]; then
        echo -e "${RED}[T-T]${NC} $1"
        cat "$DIFF_OUT"
    else
        echo -e "${GREEN}[UwU]${NC} $1"
    fi
    echo "  ________________________________________________________________"
}

testcase_as_stdin_pipe()
{
    cat "$1" | bash --posix > "$REF_OUT" 2>"$ERR_REF_OUT"
    exit_code_ref=$?
    cat "$1" | $my42sh > "$TEST_OUT" 2> "$ERR_TEST_OUT"
    exit_code_test=$?
    diff -u "$REF_OUT" "$TEST_OUT" > "$DIFF_OUT"
    if [ $? -eq 1 ] || [ $exit_code_ref -ne $exit_code_test ]; then
        echo -e "${RED}[T-T]${NC} $1"
        cat "$DIFF_OUT"
    else
        echo -e "${GREEN}[UwU]${NC} $1"
    fi
    echo "  ________________________________________________________________"

}

testcase_as_stdin_redir()
{
    bash --posix < "$1"  > "$REF_OUT" 2>"$ERR_REF_OUT"
    exit_code_ref=$?
    $my42sh < $1 > "$TEST_OUT" 2> "$ERR_TEST_OUT"
    exit_code_test=$?
    diff -u "$REF_OUT" "$TEST_OUT" > "$DIFF_OUT"
    if [ $? -eq 1 ] || [ $exit_code_ref -ne $exit_code_test ]; then
        echo -e "${RED}[T-T]${NC} $1"
        cat "$DIFF_OUT"
    else
        echo -e "${GREEN}[UwU]${NC} $1"
    fi
    echo "  ________________________________________________________________"
}

echo ">.>____________________________ARGS________________________________<.<"
#tests for args " -c"
for file in "$dir_arguments"/*.txt; do
    testcase_as_arg "$(cat $file)"
done

testcase_as_arg "echo toto"
testcase_as_arg "ls"

echo ">.>__________________________SCRIPTS_______________________________<.<"
#tests for scripts
for file in "$dir_scripts"/*; do
    testcase_as_script "$file"
done

#testcase_as_script "scripts/hello.sh"

echo ">.>___________________________STDIN________________________________<.<"
#tests for stdin 
for file in "$dir_scripts"/*; do
    testcase_as_stdin_redir "$file"
done

for file in "$dir_scripts"/*; do
    testcase_as_stdin_pipe "$file"
done

#testcase_as_stdin_pipe "scripts/hello.sh"
#testcase_as_stdin_redir "scripts/hello.sh"
