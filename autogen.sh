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

rm -f config_links.m4
for i in tests/*.sh tests/*.fee tests/*.geo tests/*.msh tests/*.dat tests/*.ref; do
 echo "AC_CONFIG_LINKS([${i}:${i}])" >> config_links.m4
done

# do not add doc to src unless they ask for it
echo -n "creating Makefile.am... "
if [ "x$1" = "x--doc" ]; then
  cat << EOF > Makefile.am
SUBDIRS = src doc
EOF

else
  cat << EOF > Makefile.am
SUBDIRS = src
EOF

fi
cat Makefile.base >> Makefile.am
touch doc/feenox-desc.texi
echo "ok"

echo "calling autoreconf... "
autoreconf -i
echo "done"
