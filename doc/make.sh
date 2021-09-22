#!/bin/bash

for i in touch sed pandoc; do
 if [ -z "$(which $i)" ]; then
  echo "error: $i not installed"
  exit 1
 fi
done

if [ ! -e "design" ]; then
  echo "error: execute from doc directory"
  exit 1
fi

# main README
echo "creating main README for Github"
cd ..
 pandoc README.md  -t gfm   -o README.markdown  \
   --standalone --toc --number-sections \
   --reference-links --reference-location=section \
   --lua-filter=doc/include-files.lua \
   --lua-filter=doc/include-code-files.lua \
   --lua-filter=doc/not-in-format.lua
   
 pandoc README.md  -t plain -o README \
   --standalone --toc --number-sections \
   --reference-links --reference-location=section \
   --lua-filter=doc/include-files.lua \
   --lua-filter=doc/include-code-files.lua \
   --lua-filter=doc/not-in-format.lua
   
  pandoc TODO.md    -t plain -o TODO
cd doc

echo "creating doc's README"
pandoc README.md  -t gfm   -o README.markdown \
  --standalone --toc --reference-links --reference-location=section \
  --lua-filter=include-files.lua \
  --lua-filter=include-code-files.lua \
  --lua-filter=not-in-format.lua



echo "creating the reference markdown from the commented sources"
echo " - keywords"
./reference.sh ../src/parser/parser.c                kw     > reference-kw.md
./reference.sh ../src/parser/parser.c                kw_dae > reference-dae-kw.md
./reference.sh ../src/parser/parser.c                kw_pde > reference-pde-kw.md
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

for i in laplace thermal; do
  echo " - ${i}"
  ./reference.sh ../src/pdes/${i}/init.c           pb_${i} > reference-${i}-pb.md
  ./reference.sh ../src/pdes/${i}/init.c           kw_${i} > reference-${i}-kw.md
  ./reference.sh ../src/pdes/${i}/init.c           va_${i} > reference-${i}-va.md
  ./reference.sh ../src/pdes/${i}/init.c           re_${i} > reference-${i}-re.md
  ./reference.sh ../src/pdes/${i}/init.c           pr_${i} > reference-${i}-pr.md
  ./reference.sh ../src/pdes/${i}/bc.c             bc_${i} > reference-${i}-bc.md
done

echo "help as markdown definition list"
./help-md.sh > help.md

echo "help as a raw txt (which is used in feenox -v)"
./help-txt.sh > help.txt

echo "unix man page"
m4 date.m4 > date.yaml
# man output cannot represent math so it will complain
pandoc -s date.yaml feenox.1.md -t man -o feenox.1 \
  --lua-filter=include-files.lua \
  --lua-filter=include-code-files.lua \
  --lua-filter=not-in-format.lua \
  --lua-filter=manfilter.lua

# this goes into the make script of the webpage
# echo "unix man page in html"
# pandoc -s -t html feenox.1 -o feenox.1.html


# # manual (to markdown and not gfm because of the heavy math)
# echo "full manual in markdown & texi"
# pandoc feenox-manual.md -t markdown-fenced_divs -o feenox-manual.markdown \
#   --standalone --toc --reference-links --reference-location=section \
#   --lua-filter=include-files.lua \
#   --lua-filter=include-code-files.lua \
#   --lua-filter=not-in-format.lua


# srs & sds
for i in touch pandoc-crossref; do
 if [ -z "$(which $i)" ]; then
  exit 0
 fi
done

echo "creating SRS and SDS markdown"
pandoc sds.md -t markdown --filter pandoc-crossref -o srs.markdown \
  --standalone --toc --reference-links --reference-location=section \
  --lua-filter=include-files.lua \
  --lua-filter=include-code-files.lua \
  --lua-filter=not-in-format.lua
  
pandoc srs.md -t markdown --filter pandoc-crossref -o srs.markdown \
  --standalone --toc --reference-links --reference-location=section \
  --lua-filter=include-files.lua \
  --lua-filter=include-code-files.lua \
  --lua-filter=not-in-format.lua 

  

 

pandoc feenox-desc.md --template template.texi -o feenox-desc.texi \
  --standalone --toc \
  --lua-filter=include-files.lua \
  --lua-filter=include-code-files.lua \
  --lua-filter=not-in-format.lua
sed -i 's/@verbatim/@smallformat\n@verbatim/' feenox-desc.texi
sed -i 's/@end verbatim/@end verbatim\n@end smallformat/' feenox-desc.texi         

for i in xelatex makeinfo texi2pdf inkscape; do
 if [ -z "$(which $i)" ]; then
  exit 0
 fi
done

for i in logo nafems-le10-problem-input lorenz; do
  inkscape --export-type=pdf ${i}.svg
done
makeinfo feenox-desc.texi > /dev/null
texi2pdf feenox-desc.texi > /dev/null
