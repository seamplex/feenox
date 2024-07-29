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
  echo "error: ${i} not installed"
  exit 1
 fi
done

# check if git is available
if test ! -d ".git"; then
  echo "error: this tree is not a repository (did you download instead of clone?)" 
  exit 1
fi

# sometimes in docker it can end up in forbidden locations like /local
git describe --all
if test $? -ne 0; then
  echo "error: git is not working (are you using /local in docker?)" 
  exit 1
fi

# clean
./autoclean.sh

# create headers from text files in doc (which in turn come from parsing the .c sources)
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


# check if there are tags
if [ ! -z "$(git tag)" ]; then
  what="tags"
else
  what="all"
fi
version=$(git describe --${what} | sed 's/-/./' | cut -d- -f1 | tr -d v)

cat << EOF > version.m4
define(feenoxversion, ${version})dnl
AC_DEFINE([FEENOX_GIT_VERSION], ["$(git describe --tags | sed 's/-/./' | tr -d \\n])"], [FeenoX Git version string])
AC_DEFINE([FEENOX_GIT_BRANCH],  ["$(git symbolic-ref HEAD | sed -e 's,.*/\(.*\),\1,' | tr -d \\n])"], [FeenoX Git branch])
AC_DEFINE([FEENOX_GIT_DATE],    ["$(git log --pretty=format:"%ad" | head -n1 | tr -d \\n])"], [FeenoX Git date])
AC_DEFINE([FEENOX_GIT_CLEAN],   ["$(git status --porcelain | wc -l | tr -d \\n])"], [FeenoX Git clean])
EOF


# these links are needed for make dist-check
rm -f config_links.m4
for i in tests/*.sh tests/*.fee tests/*.geo tests/*.msh tests/*.dat tests/*.ref tests/*.vtk tests/reed*.csv; do
 echo "AC_CONFIG_LINKS([${i}:${i}])" >> config_links.m4
done

# let's revisit this when understanding texinfo figures
# echo -n "creating Makefile.am... "
# if [ "x${1}" = "x--doc" ]; then
#   cp Makefile-doc.am Makefile.am
# else
#   echo "SUBDIRS = src" > Makefile.am
# fi

# cat Makefile-base.am >> Makefile.am
# touch doc/feenox-desc.texi
# echo "ok"


# detect the sources in pdes
echo -n "creating src/Makefile.am... "

cd src/pdes

automatic="// automatically generated by autogen.sh on $(date)"

echo ${automatic} > methods.h
echo ${automatic} > parse.c

cat << EOF >> parse.c
#include "feenox.h"

int feenox_pde_parse_problem_type(const char *token) {

///kw_pde+PROBLEM+desc Ask FeenoX to solve a partial differential equation problem.
///kw_pde+PROBLEM+usage PROBLEM
///kw_pde+PROBLEM+detail Currently, FeenoX can solve the following types of PDE-casted problems:
///kw_pde+PROBLEM+detail @
EOF

printf '#define AVAILABLE_PDES "' > available.h

first=1
for pde in *; do
 if [ -d ${pde} ]; then
  if [ -e ${pde}/methods.h ] && [ -e ${pde}/init.c ]; then

    printf '#include "%s/methods.h"\n' ${pde} >> methods.h
    printf '%s\\n' ${pde} >> available.h

    if [ ${first} -eq 0 ]; then
      sep="|"
    else
      sep="{"
    fi
  
    echo "///kw_pde+PROBLEM+usage ${sep} ${pde}" >> parse.c
    grep "///kw_pde+PROBLEM+detail" ${pde}/init.c >> parse.c

    if [ ${first} -eq 0 ]; then
      echo -n "  } else " >> parse.c
    else
      echo -n "  " >> parse.c
    fi
    cat << EOF >> parse.c
if (strcasecmp(token, "${pde}") == 0) {
    feenox.pde.parse_problem = feenox_problem_parse_problem_${pde};
    
EOF
    
    first=0
  else
    echo "warning: ${pde} does not contain methods.h and/or init.c"
  fi  
 fi
done

cat << EOF >> parse.c      
  } else {
    feenox_push_error_message("unknown problem type '%s'", token);
    return FEENOX_ERROR;
      
  }
  
///kw_pde+PROBLEM+usage }@
///kw_pde+PROBLEM+detail @
///kw_pde+PROBLEM+detail > If you are a programmer and want to contribute with another problem type, please do so!
///kw_pde+PROBLEM+detail > Check out the [programming guide in the FeenoX repository](https://github.com/seamplex/feenox/blob/main/doc/programming.md).
///kw_pde+PROBLEM+detail @
    
  return FEENOX_OK;
}
EOF
printf '"' >> available.h

cd ..

cp Makefile-base.am Makefile.am
find pdes \( -name "*.c" -o -name "*.h" \) | xargs echo -n >> Makefile.am
cd ..
echo "ok"

# docs
echo "md to txt... "
for i in TODO ChangeLog doc/programming doc/compilation doc/FAQ doc/CODE_OF_CONDUCT doc/sds doc/srs doc/feenox-manual; do
 if [ ! -e ${i} ]; then
  if [ ! -z "$(which pandoc)" ]; then
   pandoc ${i}.md -t plain -o ${i}
  else
   cp ${i}.md ${i}
  fi
 fi
done


echo "calling autoreconf... "
autoreconf -i
# for some reason, autoreconf calls autoheader which in turns overwrites
# the template config.h.in so we keep ours in config.h.ac and copy it now
cp config.h.ac config.h.in
echo "done"


