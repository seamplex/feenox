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

if test ! -z "$(which pandoc)"; then
 pandoc README.pd  -t gfm   -o README.md  --reference-links --reference-location=section
 pandoc README.md  -t plain -o README
 pandoc TODO.md    -t plain -o TODO
# else
#  cp README.pd       README.md
#  fmt -s README.md > README
#  fmt -s TODO.md   > README
fi

echo "calling autoreconf... "
autoreconf -i
echo "done"
