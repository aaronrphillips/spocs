AC_INIT([boost_filesystem_test],[0.1],[thomas@randspringer.de])
AC_PREREQ([2.59])
AC_CONFIG_AUX_DIR(config)
# Every other copy of the package version number gets its value from here
AM_INIT_AUTOMAKE
AC_PROG_CXX
#AC_PROG_CC
#AX_BOOST([1.33])
#if test "x$BOOST_REGEX_LIB" = "x"; then
#   echo "Sorry, we need the Regex-Lib from Boost."
#   echo "Please install from http://www.boost.org"
#   exit 1
#fi
#AX_BOOST
AX_BOOST_BASE([1.33])
AX_BOOST_FILESYSTEM

# create a config.h file (Automake will add -DHAVE_CONFIG_H)
AC_CONFIG_SRCDIR([file_size.cpp])
AC_CONFIG_HEADER([config.h])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
