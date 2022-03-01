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
touch ./src/variables.mak

cat << EOF > ./src/help.h
#define FEENOX_HELP_ONE_LINER     "$(cat doc/help-one-liner.txt)"
#define FEENOX_HELP_USAGE         "$(cat doc/help-usage.txt)"
#define FEENOX_HELP_OPTIONS_BASE  "\\
$(awk '{printf("%s\\n\\\n", $0)}' doc/help-options-base.txt)
"
#define FEENOX_HELP_OPTIONS_PDE   "\\
$(awk '{printf("%s\\n\\\n", $0)}' doc/help-options-pde.txt)
"
#define FEENOX_HELP_EXTRA          "\\
$(awk '{printf("%s\\n\\\n", $0)}' doc/help-extra.txt)
"
EOF


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
  cp Makefile-doc.am Makefile.am
else
  echo "SUBDIRS = src" > Makefile.am
fi

cat Makefile-base.am >> Makefile.am
touch doc/feenox-desc.texi
echo "ok"


# detect the sources in pdes
echo -n "creating src/Makefile.am... "

cd src/pdes
echo "// automatically generated in autogen.sh" > methods.h
for pde in *; do
 if [ -d ${pde} ]; then
  if [ ! -e ${pde}/methods.h ]; then
    echo "warning: ${pde} does not contain methods.h"
  else
    echo "#include \"${pde}/methods.h\"" >> methods.h
  fi  
 fi
done

cd ..

cp Makefile-base.am Makefile.am
find pdes \( -name "*.c" -o -name "*.h" \) | xargs echo -n >> Makefile.am
cd ..
echo "ok"

echo "calling autoreconf... "
autoreconf -i
echo "done"
