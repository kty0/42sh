autoreconf -i
./configure CFLAGS="--coverage"
make check
rm a-conftest.gcno
gcovr --html --html-nested --output=report.html
find . -type f -name '*.html' ! -name 'report.html' -delete
