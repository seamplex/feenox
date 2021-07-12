#!/bin/sh
# 
# Execute this script to generate a configure script
#
# This file is free software: you are free to change and redistribute it.
# There is NO WARRANTY, to the extent permitted by law.
#

# check for needed tools
for i in git autoconf m4 make; do
 if [ -z "$(which $i)" ]; then
  echo "error: $i not installed"
  exit 1
 fi
done

if test ! -d ".git"; then
  echo "error: this tree is not a repository (did you download instead of clone?)" 
  exit 1
fi

./autoclean.sh
touch src/variables.mak

# echo "#define FEENOX_ONE_LINER \"desc\"" | m4 -Ddesc="$(cat doc/one-liner.md)" > src/help.h

# major version is equal to the latest tag
version=$(git describe --tags | sed 's/-/./')
echo "define(feenoxversion, ${version})dnl" > version.m4

# if [ ! -e README ]; then
#  touch README
# fi
# if [ ! -e TODO ]; then
#  touch TODO
# fi

echo "calling autoreconf... "
autoreconf -i
# automake --add-missing
echo "done"
