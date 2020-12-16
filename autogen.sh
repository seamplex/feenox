#!/bin/sh
# 
# Execute this script to generate a configure script
#
# This file is free software: you are free to change and redistribute it.
# There is NO WARRANTY, to the extent permitted by law.
#

# check for needed tools (cannot put this into an m4 macro
# because we do not even know if we have m4 available)
for i in git autoconf m4 make; do
 if [ -z "$(which $i)" ]; then
  echo "error: $i not installed"
  exit 1
 fi
done

./autoclean.sh

if test ! -d ".git"; then
  echo "error: this tree is not a repository (did you download instead of clone?)" 
  exit 1
fi

# major version is equal to the latest tag
version=$(git describe --tags | sed 's/-/./')
echo "define(feenoxversion, ${version})dnl" > version.m4

branch=$(git symbolic-ref HEAD | sed -e 's,.*/\(.*\),\1,')
commitdate=$(git log -1 --pretty=format:"%ad")

cat << EOF > src/version-vcs.h
#define FEENOX_GIT_BRANCH    "${branch}"
#define FEENOX_GIT_VERSION   "${version}"
#define FEENOX_GIT_DATE      "${commitdate}"
#define FEENOX_GIT_CLEAN     $(git status --porcelain | wc -l)
EOF

if test ! -z "$(which pandoc)"; then
 pandoc README.md  -t plain -o README
#  pandoc INSTALL.md -t plain -o INSTALL
else
 fmt -s README.md > README
#  fmt -s INSTALL.md > INSTALL
fi


echo "calling autoreconf... "
autoreconf -i 2>&1
echo "done"
