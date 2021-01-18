#!/bin/bash
# 
# Execute this script to clean the directory and leave it as
# a fresh mercurial branch
#
if test -e Makefile; then
  make clean
fi
rm -f *~ .*~ src/*~
rm -f src/variables.mak
cat .gitignore | grep -v nbproject | sed '/^#.*/ d' | sed '/^\s*$/ d' | sed 's/^/rm -rf /' | bash
