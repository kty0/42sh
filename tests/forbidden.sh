RED='\033[0;31m'
NC='\033[0m'

grep -lr --include \*.c 'glob\|regexec\|wordexp\|popen\|syscall\|system' ../ > /tmp/forbidden.out

[ $(wc -c < /tmp/forbidden.out) -eq 0 ]
res=$?

if [ $res -ne 0 ]; then
    echo -e "${RED}Forbidden function(s) found in"
    cat /tmp/forbidden.out
    echo -en "${NC}"
fi

exit $res
