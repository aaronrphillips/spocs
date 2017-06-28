#!/bin/bash

#  we are using version 1.53.0.
#  NOTE: Later versions may have new or different build and installation instructions.
#  we are using the bash shell on Linux
#  we will build the libraries in BUILD_DIR, /var/tmp
#  we will install the libraries in INSTALL_DIR, /usr/local
#  we are building the 64-bit versions of the libaries
#  we are building "static" (LIB_MODE) version of the libraries
#  we are building "release" (BOOST_VARIANT) versions of the libaries

export CXX_PATH=/usr
#export CXX_PATH=/usr/local/gcc_4_7_1 # Has C++11 features and speed
export CXX=$CXX_PATH/bin/g++
export INSTALL_DIR=/usr/local
export LIB_MODE="static"              # Change to "shared" for shared libraries
export BOOST_VARIANT="release"        # Change to "debug"  for debug  libraries

cd /var/tmp

wget "http://downloads.sourceforge.net/project/boost/boost/1.53.0/boost_1_53_0.tar.gz?r=http%3A%2F%2Fsourceforge.net%2Fprojects%2Fboost%2Ffiles%2Fboost%2F1.53.0%2F&ts=1362095108&use_mirror=voxel"

tar xzf boost_1_53_0.tar.gz
cd boost_1_53_0
export BOOST_VERSION=`basename $PWD`

./bootstrap.sh --with-libraries=all -without-libraries=mpi --without-icu

./bjam -j6 --layout=tagged --without-mpi --without-python --builddir=../build_boost --build-type=complete toolset=gcc --prefix=$INSTALL_DIR/${BOOST_VERSION}_${LIB_MODE}64_${BOOST_VARIANT}_multi optimization=speed link=$LIB_MODE address-model=64 runtime-link=$LIB_MODE variant=${BOOST_VARIANT} threading=multi install > build_boost.out 2>&1

exit $?
