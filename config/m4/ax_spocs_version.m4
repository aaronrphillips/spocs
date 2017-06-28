##### http://autoconf-archive.cryp.to/ax_boost_base.html
#
# SYNOPSIS
#
#   AX_SPOCS_VERSION
#
# DESCRIPTION
#
#     AC_SUBST(SPOCS_VERSION)
#
#   And sets:
#
#     SPOCSVERSION
#
# LAST MODIFICATION
#
#   2013-03-15
#

AC_DEFUN([AX_SPOCS_VERSION],
[AC_MSG_CHECKING([for SPOCS version])

  TOP=$1
  TOP_CONF="$TOP/conf"
  TOP_DATA="$TOP/data"
  DIST_DIR="/srv/lcs/warehouse/spocs"

  VERSION=`cat $TOP/VERSION`
  NEW_MAJOR_VERSION=`cat $TOP/VERSION | awk -F'_' '{print $[1]+1}' | bc`
  NEW_MINOR_VERSION=`cat $TOP/VERSION | awk -F'_' '{print $[2]+1}' | bc`
  NEW_MICRO_VERSION=`cat $TOP/VERSION | awk -F'_' '{print $[3]+1}' | bc`
  MAJOR_VERSION=`cat $TOP/VERSION | awk -F'_' '{print $[1]}'   | bc`
  MINOR_VERSION=`cat $TOP/VERSION | awk -F'_' '{print $[2]}'   | bc`
  MICRO_VERSION=`cat $TOP/VERSION | awk -F'_' '{print $[3]}'   | bc`
  MAJOR=`echo ${NEW_MAJOR_VERSION}_${MINOR_VERSION}_${MICRO_VERSION}`
  MINOR=`echo ${MAJOR_VERSION}_${NEW_MINOR_VERSION}_${MICRO_VERSION}`
  MICRO=`echo ${MAJOR_VERSION}_${MINOR_VERSION}_${NEW_MICRO_VERSION}`
  CXX_VENDOR="GNU"
  CXX_OPTIMIZE_LEVEL=`echo $CXXFLAGS | sed -e 's/.*\-O/O/' -e 's/ .*//'`
  CXX_DEBUG_LEVEL=`echo $CXXFLAGS | grep -c '\-g' | sed -e 's/1/ON/' -e 's/0/OFF/'`
  CXX_VERSION=`$CXX -v 2>&1 | grep -i " version" | awk -F' ' '{print $[3]}'`
  
  CXXVERDEF="-D'CXX_VER=\"$CXX_VENDOR $CXX -v $CXX_VERSION\"'"
  CXXOPTDEF="-D'CXX_OPTIMIZE_LEVEL=\"$CXX_OPTIMIZE_LEVEL\"'"
  CXXDBGDEF="-D'CXX_DEBUG_LEVEL=\"$CXX_DEBUG_LEVEL\"'"

  SPOCS_CXX_STATIC=`$CPP -dM $TOP/src/spocs.hpp | grep -c __APPLE__ | sed -e "s/1/ /" -e "s/0/ -static /"`
  SPOCS_CXX=`which $CXX`
  SPOCS_CXX_DIR=`dirname $SPOCS_CXX`
  SPOCS_CXX_ROOT=`dirname $SPOCS_CXX_DIR`
  SPOCS_CXX_INCLUDE=`echo "-I$SPOCS_CXX_ROOT/include"`
  SPOCS_CXX_LIB=`echo "-L$SPOCS_CXX_ROOT/lib"`


  SPOCS_VERSION="-D'SPOCS_VERSION=\"$VERSION\"' $CXXVERDEF $CXXOPTDEF $CXXDBGDEF"

  AC_SUBST(SPOCS_CXX_STATIC)
  AC_SUBST(SPOCS_CXX_INCLUDE)
  AC_SUBST(SPOCS_CXX_LIB)
  AC_SUBST(SPOCS_VERSION)

AC_MSG_RESULT([$SPOCS_VERSION])
])
