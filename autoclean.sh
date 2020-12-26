#!/bin/bash
# 
# Execute this script to clean the directory and leave it as
# a fresh mercurial branch
#
if test -e Makefile; then
  make clean
fi
rm -f *~ .*~ src/*~
cat .gitignore | sed '/^#.*/ d' | sed '/^\s*$/ d' | sed 's/^/rm -rf /' | bash
rm -f src/petscslepc.mak
