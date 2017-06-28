#!/bin/bash

#
#  DEVELOPER NOTE: Need to setup Google Test libraries
#
top_dir=`pwd`

cd support/vendorsrc/gtest-1.6.0
./configure
# --prefix=$top_dir

make
cd make
make
mkdir -p $top_dir/lib > /dev/null 2>&1
cp *.a $top_dir/lib
