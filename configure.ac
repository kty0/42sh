# Init the 42sh project
AC_INIT([42sh], [1.0], [julie.duscastel@epita.fr])

# Setup automake
AM_INIT_AUTOMAKE([subdir-objects] [foreign])

# Pretty display of makefile rules
AM_SILENT_RULES([yes])

# Enable ar for Makefile
AM_PROG_AR

# Check if a ranlib is available
AC_PROG_RANLIB

# Check if a compiler is available for c
AC_PROG_CC

# List Makefile in subdirectories
AC_CONFIG_FILES([
                 Makefile
                 src/Makefile
                 src/ast/Makefile
                 src/parser/Makefile
                 src/lexer/Makefile
                 src/built_in/Makefile
                 src/eval/Makefile
                 src/domain_expansion/Makefile
                 src/hash_map/Makefile
                 tests/Makefile
                 ])
AC_OUTPUT
