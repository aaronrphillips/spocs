#!/bin/bash

. $HOME/.bashrc

#ncpu=`grep processor /proc/cpuinfo | wc -l`
make distclean
rm -rf autom4te.cache
rm -f  config.log config.status Makefile
rm -f  bin/*
autoreconf --install --force 
./configure --prefix=$HOME/dev/cliques --with-boost-lib=/usr/lib/x86_64-linux-gnu 
make
