#!/bin/bash
# 
# Execute this script to clean the directory and leave it as
# a fresh git repository branch
#
if test -e Makefile; then
  make clean
fi
rm -f *~ .*~ src/*~
cat .gitignore | grep -v nbproject | grep -v gsl | grep -v Stanford_Bunny.stl | sed '/^#.*/ d' | sed '/^\s*$/ d' | sed 's/^/rm -rf /' | bash
cd tests
cat .gitignore | grep -v nafems | sed '/^#.*/ d' | sed '/^\s*$/ d' | sed 's/^/rm -rf /' | bash
cd ..

