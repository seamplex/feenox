#!/bin/sh
# 
# Execute this script to clean the directory and leave it as
# a fresh mercurial branch
#
if test -e Makefile; then
  make clean
fi
rm -f *~ .*~ src/*~
