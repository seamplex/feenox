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
  cat << EOF > Makefile.am
SUBDIRS = src doc

dist_doc_DATA = AUTHORS ChangeLog TODO COPYING \\
                README README.markdown \\
                doc/feenox-desc.texi \\
                doc/feenox.xml \\
                doc/README.md \\
                doc/CODE_OF_CONDUCT \\
                doc/programming \\
                doc/compilation \\
                doc/FAQ \\
                doc/syntax-kate.sh \\
                doc/syntax-tex.sh \\
                doc/nafems-le10-problem-input.eps \\
                doc/lorenz.eps \\
                doc/cantilever-displacement.eps \\
                doc/fork.eps \\
                doc/fork-meshed.eps \\
                doc/laplace-square-gmsh.eps \\
                doc/laplace-square-paraview.eps \\
                doc/nafems-le10.eps
                
dist_man_MANS = doc/feenox.1
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
