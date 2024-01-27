#!/bin/sh

REF_OUT="/tmp/bash.out"
TEST_OUT="/tmp/42sh.out"
DIFF_OUT="/tmp/diff.out"
ERR_REF_OUT="/tmp/err.out"
ERR_TEST_OUT="/tmp/err_test.out"

F1="file1"
F2="file2"
RTEST1="/tmp/rtest1.out"
RTEST2="/tmp/rtest2.out"
RREF1="/tmp/rref1.out"
RREF2="/tmp/rref2.out"

RED='\033[0;31m'
GREEN='\033[0;32m'
GRAY='\033[0;37m'
BLUE='\033[0;34m'
NC='\033[0m'

my42sh="../src/./42debug"

if [ "$1" = "--show" ] || [ "$1" = "-s" ]; then
    do_print=0
elif [ "$1" = "--hide" ] || [ "$1" = "-h" ]; then
    do_print=1
else
    echo "usage: ./testsuite.sh (--hide|--show|-h|-s) [--fails|-f] [--error|-e]"
    exit 1
fi

show_suc=0
show_err=0
for arg in "$@"
do
    if [ $arg = "--fails" ] || [ $arg = "-f" ]; then
        show_suc=1
    elif [ $arg = "--error" ] || [ $arg = "-e" ]; then
        show_err=1
    fi
done


nb_tested=0
nb_ko=0

testcase_redir()
{
    > "$DIFF_OUT"
    nb_tested=$(($nb_tested+1))
    file=$(cat $1)

    rm $F1 2> /dev/null
    rm $F2 2> /dev/null

    bash --posix -c "$file" > "$REF_OUT" 2> "$ERR_REF_OUT"
    exit_code_ref=$?

    [ -s $F1 ]
    xref1=$(echo $?)
    [ -s $F2 ]
    xref2=$(echo $?)
    cat $F1 > $RREF1 2> /dev/null
    cat $F2 > $RREF2 2> /dev/null

    rm $F1 2> /dev/null
    rm $F2 2> /dev/null

    $my42sh -c "$file" > "$TEST_OUT" 2> "$ERR_TEST_OUT"
    exit_code_test=$?

    [ -s $F1 ]
    xtest1=$(echo $?)
    [ -s $F2 ]
    xtest2=$(echo $?)
    cat $F1 > $RTEST1 2> /dev/null
    cat $F2 > $RTEST2 2> /dev/null

    rm $F1 2> /dev/null
    rm $F2 2> /dev/null

    diff -u "$RTEST1" "$RREF1" >> "$DIFF_OUT"
    code1=$(echo $?)
    diff -u "$RTEST2" "$RREF2" >> "$DIFF_OUT"
    code2=$(echo $?)
    diff --color -u "$TEST_OUT" "$REF_OUT" >> "$DIFF_OUT"
    exit_diff=$?

    err_diff=1
    if [ $(wc -c < $ERR_REF_OUT) -eq 0 ] && [ $(wc -c < $ERR_TEST_OUT) -eq 0 ]; then
        err_diff=0
    elif [ $(wc -c < $ERR_REF_OUT) -ne 0 ] && [ $(wc -c < $ERR_TEST_OUT) -ne 0 ]; then
        err_diff=0
    fi

    if [ $err_diff -eq 1 ] || [ $xtest1 -eq $xref1 ] || [ $xtest2 -eq $xref2 ] || [ $code1 -eq 1 ] || [ $code2 -eq 1 ] || [ $exit_code_ref -ne $exit_code_test ] || [ $err_diff -eq 1 ]; then
        echo -e "${RED}[T-T]${NC} $1"
        if [ $do_print -eq 0 ]; then
            cat "$DIFF_OUT"
            if [ $exit_code_ref -ne $exit_code_test ]; then
                echo "Expected exit code: $exit_code_ref"
                echo "But got exit code: $exit_code_test"
            fi
            if [ $show_err -eq 1 ]; then
                cat "$ERR_TEST_OUT"
            fi
        fi
        nb_ko=$(($nb_ko + 1))
        echo "  ________________________________________________________________\n"
    elif [ $show_suc -eq 0 ]; then
        echo -e "${GREEN}[UwU]${NC} $1"
        echo "  ________________________________________________________________\n"
    fi
}

testcase_as_arg()
{
    nb_tested=$(($nb_tested+1))
    file=$(cat $1)
    bash --posix -c "$file" > "$REF_OUT" 2> "$ERR_REF_OUT"
    exit_code_ref=$?
    $my42sh -c "$file" > "$TEST_OUT" 2> "$ERR_TEST_OUT"
    exit_code_test=$?
    diff --color -u "$TEST_OUT" "$REF_OUT" > "$DIFF_OUT"
    exit_diff=$?

    err_diff=1
    if [ $(wc -c < $ERR_REF_OUT) -eq 0 ] && [ $(wc -c < $ERR_TEST_OUT) -eq 0 ]; then
        err_diff=0
    elif [ $(wc -c < $ERR_REF_OUT) -ne 0 ] && [ $(wc -c < $ERR_TEST_OUT) -ne 0 ]; then
        err_diff=0
    fi

    if [ $exit_diff -eq 1 ] || [ $exit_code_ref -ne $exit_code_test ] || [ $err_diff -eq 1 ]; then
        echo -e "${RED}[T-T]${NC} $1"
        if [ $do_print -eq 0 ]; then
            cat "$DIFF_OUT"
            if [ $exit_code_ref -ne $exit_code_test ]; then
                echo "Expected exit code: $exit_code_ref"
                echo "But got exit code: $exit_code_test"
            fi
            if [ $show_err -eq 1 ]; then
                cat "$ERR_TEST_OUT"
            fi
        fi
        nb_ko=$(($nb_ko + 1))
        echo "  ________________________________________________________________\n"
    elif [ $show_suc -eq 0 ]; then
        echo -e "${GREEN}[UwU]${NC} $1"
        echo "  ________________________________________________________________\n"
    fi
}

testcase_as_script()
{
    nb_tested=$(($nb_tested+1))
    bash --posix "$1" > "$REF_OUT" 2> "$ERR_REF_OUT"
    exit_code_ref=$?
    $my42sh $1 > "$TEST_OUT" 2> "$ERR_TEST_OUT"
    exit_code_test=$?
    diff --color -u "$TEST_OUT" "$REF_OUT" > "$DIFF_OUT"
    exit_diff=$?

    err_diff=1
    if [ $(wc -c < $ERR_REF_OUT) -eq 0 ] && [ $(wc -c < $ERR_TEST_OUT) -eq 0 ]; then
        err_diff=0
    elif [ $(wc -c < $ERR_REF_OUT) -ne 0 ] && [ $(wc -c < $ERR_TEST_OUT) -ne 0 ]; then
        err_diff=0
    fi

    if [ $exit_diff -eq 1 ] || [ $exit_code_ref -ne $exit_code_test ] || [ $err_diff -eq 1 ]; then
        echo -e "${RED}[T-T]${NC} $1"
        if [ $do_print -eq 0 ]; then
            cat "$DIFF_OUT"
            if [ $exit_code_ref -ne $exit_code_test ]; then
                echo "Expected exit code: $exit_code_ref"
                echo "But got exit code: $exit_code_test"
            fi
            if [ $show_err -eq 1 ]; then
                cat "$ERR_TEST_OUT"
            fi
        fi
        nb_ko=$(($nb_ko + 1))
        echo "  ________________________________________________________________\n"
    elif [ $show_suc -eq 0 ]; then
        echo -e "${GREEN}[UwU]${NC} $1"
        echo "  ________________________________________________________________\n"
    fi
}


testcase_as_stdin_pipe()
{
    nb_tested=$(($nb_tested+1))
    cat "$1" | bash --posix > "$REF_OUT" 2> "$ERR_REF_OUT"
    exit_code_ref=$?
    cat "$1" | $my42sh > "$TEST_OUT" 2> "$ERR_TEST_OUT"
    exit_code_test=$?
    diff --color -u "$TEST_OUT" "$REF_OUT" > "$DIFF_OUT"
    exit_diff=$?

    err_diff=1
    if [ $(wc -c < $ERR_REF_OUT) -eq 0 ] && [ $(wc -c < $ERR_TEST_OUT) -eq 0 ]; then
        err_diff=0
    elif [ $(wc -c < $ERR_REF_OUT) -ne 0 ] && [ $(wc -c < $ERR_TEST_OUT) -ne 0 ]; then
        err_diff=0
    fi

    if [ $exit_diff -eq 1 ] || [ $exit_code_ref -ne $exit_code_test ] || [ $err_diff -eq 1 ]; then
        echo -e "${RED}[T-T]${NC} $1"
        if [ $do_print -eq 0 ]; then
            cat "$DIFF_OUT"
            if [ $exit_code_ref -ne $exit_code_test ]; then
                echo "Expected exit code: $exit_code_ref"
                echo "But got exit code: $exit_code_test"
            fi
            if [ $show_err -eq 1 ]; then
                cat "$ERR_TEST_OUT"
            fi
        fi
        nb_ko=$(($nb_ko + 1))
        echo "  ________________________________________________________________\n"
    elif [ $show_suc -eq 0 ]; then
        echo -e "${GREEN}[UwU]${NC} $1"
        echo "  ________________________________________________________________\n"
    fi

}

testcase_as_stdin_redir()
{
    nb_tested=$(($nb_tested+1))
    bash --posix < "$1" > "$REF_OUT" 2> "$ERR_REF_OUT"
    exit_code_ref=$?
    $my42sh < "$1" > "$TEST_OUT" 2> "$ERR_TEST_OUT"
    exit_code_test=$?
    diff --color -u "$TEST_OUT" "$REF_OUT" > "$DIFF_OUT"
    exit_diff=$?

    err_diff=1
    if [ $(wc -c < $ERR_REF_OUT) -eq 0 ] && [ $(wc -c < $ERR_TEST_OUT) -eq 0 ]; then
        err_diff=0
    elif [ $(wc -c < $ERR_REF_OUT) -ne 0 ] && [ $(wc -c < $ERR_TEST_OUT) -ne 0 ]; then
        err_diff=0
    fi

    if [ $exit_diff -eq 1 ] || [ $exit_code_ref -ne $exit_code_test ] || [ $err_diff -eq 1 ]; then
        echo -e "${RED}[T-T]${NC} $1"
        if [ $do_print -eq 0 ]; then
            cat "$DIFF_OUT"
            if [ $exit_code_ref -ne $exit_code_test ]; then
                echo "Expected exit code: $exit_code_ref"
                echo "But got exit code: $exit_code_test"
            fi
            if [ $show_err -eq 1 ]; then
                cat "$ERR_TEST_OUT"
            fi
        fi
        nb_ko=$(($nb_ko + 1))
        echo "  ________________________________________________________________\n"
    elif [ $show_suc -eq 0 ]; then
        echo -e "${GREEN}[UwU]${NC} $1"
        echo "  ________________________________________________________________\n"
    fi
}

echo "\n\n\n  >.> ________________________ ARGS _________________________ <.<\n"
#tests for args " -c"

for file in $(find "piscine-scripts" -name "*.sh"); do
    testcase_as_arg "$file"
done

for file in $(find "step1/" -name "*.sh"); do
    testcase_as_arg "$file"
done

for file in $(find "step2/" -name "*.sh"); do
    testcase_as_arg "$file"
done

for file in $(find "redirections/" -name "*.sh"); do
    testcase_redir "$file"
done

echo "\n\n\n  >.> ________________________ SCRIPTS _________________________ <.<\n"
#tests for scripts

for file in $(find "piscine-scripts/" -name "*.sh"); do
    testcase_as_script "$file"
done

for file in $(find "step1/" -name "*.sh"); do
    testcase_as_script "$file"
done

for file in $(find "step2/" -name "*.sh"); do
    testcase_as_script "$file"
done

echo "\n\n\n  >.> ________________________ STDIN _________________________ <.<\n"
#tests for stdin

for file in $(find "piscine-scripts/" -name "*.sh"); do
    testcase_as_stdin_pipe "$file"
done

for file in $(find "step1/" -name "*.sh"); do
    testcase_as_stdin_pipe "$file"
done

for file in $(find "step2/" -name "*.sh"); do
    testcase_as_stdin_pipe "$file"
done

# REDIRs

for file in $(find "piscine-scripts/" -name "*.sh"); do
    testcase_as_stdin_redir "$file"
done

for file in $(find "step1/" -name "*.sh"); do
    testcase_as_stdin_redir "$file"
done

for file in $(find "step2/" -name "*.sh"); do
    testcase_as_stdin_redir "$file"
done

echo -e "Recap: Tested: ${BLUE}${nb_tested}${NC} | Passed: ${GREEN}$(($nb_tested - $nb_ko))${NC} | Failed: ${RED}${nb_ko}${NC} | Failure: ${RED}$(($nb_ko * 100 / $nb_tested))${NC}%"
