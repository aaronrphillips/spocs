AC_INIT([spocs], [`cat VERSION`], [leeann.mccue@pnnl.gov])
AC_PREREQ([2.59])
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_SRCDIR([src/spocs.cpp])
AC_CONFIG_HEADERS([src/config.h:src/config.h.in])
AM_MAINTAINER_MODE


AM_CONDITIONAL(MAINTAINER, test "$enable_maintainer_mode" = yes)

#
#  Checks for programs.
#
AC_PATH_PROG([BLASTALL], [blastall], [no])
if test x"${BLASTALL}" == x"no" ; then
  AC_MSG_ERROR(Could not find NCBI blastall in your path!)
fi

AC_PROG_CC([cc])
if test x"${CC}" == x"no" ; then
  AC_MSG_ERROR(Could not find GNU cc in your path!)
fi

AC_PROG_CPP([cpp])
AC_PATH_PROG([CPP], [cpp], [no])
if test x"${CPP}" == x"no" ; then
  AC_MSG_ERROR(Could not find GNU cpp in your path!)
fi

AC_PROG_CXX([g++])
AC_PATH_PROG([CXX], [g++], [no])
if test x"${CXX}" == x"no" ; then
  AC_MSG_ERROR(Could not find GNU g++ in your path!)
fi


#
# Checks for libraries.
#
AC_CHECK_LIB([pthread], [pthread_self])


#
#  Checks for header files.
#
AC_CHECK_HEADERS([float.h libgen.h stdlib.h string.h syslog.h unistd.h])


#
#  Checks for typedefs, structures, and compiler characteristics.
#
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_INT8_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT32_T


#
# Checks for library functions.
#
AC_FUNC_FORK


AM_INIT_AUTOMAKE([foreign -Wall -Werror subdir-objects tar-ustar])


#
# Checks for Boost library functions.
#
m4_include([config/m4/ax_boost_base.m4])
m4_include([config/m4/ax_boost_program_options.m4])
m4_include([config/m4/ax_boost_date_time.m4])
m4_include([config/m4/ax_boost_filesystem.m4])
m4_include([config/m4/ax_boost_regex.m4])
#m4_include([config/m4/ax_boost_mpi.m4])
m4_include([config/m4/ax_boost_serialization.m4])
m4_include([config/m4/ax_boost_system.m4])
#m4_include([config/m4/ax_boost_test_exec_monitor.m4])
#m4_include([config/m4/ax_boost_unit_test_framework.m4])
m4_include([config/m4/ax_spocs_version.m4])


AX_BOOST_BASE([1.33])
#AX_BOOST_PROGRAM_OPTIONS
#AX_BOOST_DATE_TIME
#AX_BOOST_FILESYSTEM
#AX_BOOST_REGEX
##AX_BOOST_MPI
#AX_BOOST_SERIALIZATION
#AX_BOOST_SYSTEM
##AX_BOOST_TEST_EXEC_MONITOR
##AX_BOOST_UNIT_TEST_FRAMEWORK


AX_SPOCS_VERSION([./])


AC_CONFIG_FILES([Makefile src/Makefile])
#test/Makefile])
# src/test/Makefile])
AC_OUTPUT
