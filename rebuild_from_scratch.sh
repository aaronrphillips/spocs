#!/bin/bash

. $HOME/.bashrc

#ncpu=`grep processor /proc/cpuinfo | wc -l`
make distclean
rm -rf autom4te.cache
rm -f  config.log config.status Makefile
rm -f  bin/*
autoreconf --install --force 
#./configure --enable-maintainer-mode --prefix=$HOME/dev/cliques --with-boost=/usr/local/boost_1_53_0_static64_release_multi
./configure --prefix=$HOME/dev/cliques --with-boost=/usr/local/boost_1_53_0_static64_release_multi
#make -j $ncpu && make -j $ncpu release && make -j $ncpu check && make -j $ncpu install
make
#&& make install
#cd src/test
#make
#make -j $ncpu && make check
