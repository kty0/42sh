autoreconf -i
./configure CFLAGS="--coverage"
make clean
make check
rm a-conftest.gcno
gcovr --html --html-nested --output=report.html
make clean
