#!/bin/bash -e

for i in feenox touch sed pandoc makeinfo inkscape convert pandoc-crossref; do
 if [ -z "$(which $i)" ]; then
  echo "error: ${i} not installed"
  exit 1
 fi
done

pdf=0
if [ "x${1}" == "x--no-pdf" ]; then
  pdf=0
fi
if [ "x${1}" == "x--pdf" ]; then
  pdf=1
fi

if [ ${pdf} = 1 ]; then
 for i in xelatex texi2pdf; do
  if [ -z "$(which $i)" ]; then
   echo "error: ${i} not installed"
   exit 1
  fi
 done
fi


if [ ! -e "design" ]; then
  echo "error: execute from doc directory"
  exit 1
fi



# main README & TODO
echo "creating main READMEs for Github"
for i in README README4engineers README4hackers README4academics; do
 echo "${i}"
 if [ ${pdf} = 1 ]; then
   ./md2.sh --pdf   ../${i}
 fi
 ./md2.sh --gfm   ../${i}
 ./md2.sh --plain ../${i}
 ./md2.sh --html  ../${i}
done

echo "download"
./md2.sh --html  ../download.md
echo "TODO"
./md2.sh --plain ../TODO

echo "creating doc's README"
if [ ${pdf} = 1 ]; then
  ./md2.sh --pdf   README
fi
./md2.sh --gfm   README
./md2.sh --plain README
./md2.sh --html  README

echo "creating the reference markdown from the commented sources"
echo " - keywords"
./reference.sh ../src/parser/parser.c                kw     > reference-kw.md
./reference.sh ../src/parser/parser.c                kw_dae > reference-dae-kw.md
cat ../src/pdes/parse.c ../src/parser/parser.c > tmp
./reference.sh tmp                                   kw_pde > reference-pde-kw.md
rm tmp
echo " - variables"
./reference.sh ../src/flow/init.c                    va     > reference-va.md
./reference.sh ../src/flow/init.c                    va_dae > reference-dae-va.md
./reference.sh ../src/flow/init.c                    va_pde > reference-pde-va.md
echo " - functions"
./reference.sh ../src/math/builtin_functions.c       fn > reference-fn.md
echo " - functionals"
./reference.sh ../src/math/builtin_functionals.c     fu > reference-fu.md
echo " - vector functions"
./reference.sh ../src/math/builtin_vectorfunctions.c fv > reference-fv.md

for i in laplace thermal mechanical neutron_diffusion neutron_sn; do
  echo " - ${i}"
  ./reference.sh ../src/pdes/${i}/init.c           pb_${i} > reference-${i}-pb.md
  ./reference.sh ../src/pdes/${i}/parser.c         kw_${i} > reference-${i}-kw.md
  ./reference.sh ../src/pdes/${i}/init.c           va_${i} > reference-${i}-va.md
  ./reference.sh ../src/pdes/${i}/init.c           re_${i} > reference-${i}-re.md
  ./reference.sh ../src/pdes/${i}/init.c           pr_${i} > reference-${i}-pr.md
  ./reference.sh ../src/pdes/${i}/bc.c             bc_${i} > reference-${i}-bc.md
done

echo "help as markdown definition list"
./help-md.sh > help.md

echo "help as a raw txt (which is used in feenox -h)"
./help-txt.sh usage   ../src/flow/init.c > help-usage.txt
./help-options-txt.sh ../src/flow/init.c > help-options-base.txt
./help-options-txt.sh ../src/pdes/init.c > help-options-pde.txt
./help-txt.sh extra   ../src/flow/init.c > help-extra.txt

echo "unix man page"
m4 date.m4 > date.yaml
pandoc -s date.yaml feenox.1.md -t man -o feenox.1 \
  --quiet \
  --lua-filter=include-files.lua \
  --lua-filter=include-code-files.lua \
  --lua-filter=not-in-format.lua \
  --lua-filter=only-in-format.lua \
  --lua-filter=img-width.lua \
  --lua-filter=manfilter.lua

echo "unix man page in html"
pandoc --quiet -s -t html feenox.1 -o feenox.1.html

# this one goes into the windows zip
pandoc --quiet -s double-click.md -t plain -o double-click.txt --lua-filter=include-files.lua



echo "PDF + HTML + Markdown + plain"
for i in programming compilation FAQ CODE_OF_CONDUCT history; do
 echo ${i}
 if [ ${pdf} = 1 ]; then
  ./md2.sh --pdf  ${i}
 fi
 ./md2.sh --html  ${i}
 ./md2.sh --gfm   ${i}
 ./md2.sh --plain ${i}
done

echo "PDF + HTML + plain"
for i in feenox-manual srs sds; do
 echo ${i}
 if [ ${pdf} = 1 ]; then
  ./md2.sh --pdf  ${i}
 fi
 ./md2.sh --html  ${i}
 ./md2.sh --plain ${i}
done


echo "feenox-desc as Texi"
pandoc feenox-desc.md --template template.texi -o feenox-desc.texi \
  --quiet \
  --standalone --toc \
  --lua-filter=include-files.lua \
  --lua-filter=include-code-files.lua \
  --lua-filter=not-in-format.lua \
  --lua-filter=only-in-format.lua \
  --lua-filter=img-width.lua

# TODO: as a lua filter
sed -i 's/@verbatim/@smallformat\n@verbatim/' feenox-desc.texi
sed -i 's/@end verbatim/@end verbatim\n@end smallformat/' feenox-desc.texi         

# TODO: as a lua filter
for i in logo \
         nafems-le10-problem-input \
         lorenz \
         cantilever-displacement \
         fork \
         fork-meshed \
  ; do
  if [ ! -e $i.pdf ]; then
    inkscape --export-type=pdf ${i}.svg
  fi
  if [ ! -e $i.eps ]; then
    inkscape --export-type=eps ${i}.svg
  fi
done

for i in laplace-square-gmsh \
         laplace-square-paraview \
         nafems-le10 \
  ; do
  if [ ! -e $i.eps ]; then
    convert ${i}.png ${i}.eps
  fi
done

echo "  makeinfo"
makeinfo feenox-desc.texi > /dev/null
if [ ${pdf} = 1 ]; then
 echo "  texi2pdf"
 texi2pdf feenox-desc.texi > /dev/null
fi 

# copy the README to README-doc because when using Makefile-doc.am there are two READMEs,
# one for the rood and one for the doc and that cannot happen when doing make distcheck
cp README README-doc

echo "it's all good man"
