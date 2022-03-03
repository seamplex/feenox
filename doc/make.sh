#!/bin/bash -e

for i in feenox touch sed pandoc xelatex makeinfo texi2pdf inkscape convert pandoc-crossref; do
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
 # TODO: function to call pandoc
 pandoc README.md -f markdown+emoji -t gfm   -o README.markdown  \
   --standalone --toc --number-sections \
   --reference-links --reference-location=section \
   --lua-filter=doc/include-files.lua \
   --lua-filter=doc/include-code-files.lua \
   --lua-filter=doc/not-in-format.lua \
   --lua-filter=doc/only-in-format.lua \
   --lua-filter=doc/img-width.lua
   
 pandoc README.md  -f markdown+emoji  -t plain -o README \
   --standalone --toc --number-sections \
   --reference-links --reference-location=section \
   --lua-filter=doc/include-files.lua \
   --lua-filter=doc/include-code-files.lua \
   --lua-filter=doc/not-in-format.lua \
   --lua-filter=doc/only-in-format.lua \
   --lua-filter=doc/img-width.lua
   
  pandoc TODO.md    -t plain -o TODO
cd doc

echo "creating doc's README"
pandoc README.md  -t gfm   -o README.markdown \
  --standalone --toc --reference-links --reference-location=section \
  --lua-filter=include-files.lua \
  --lua-filter=include-code-files.lua \
  --lua-filter=not-in-format.lua \
  --lua-filter=only-in-format.lua \
  --lua-filter=img-width.lua



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

echo "help as a raw txt (which is used in feenox -h)"
./help-txt.sh usage   ../src/flow/init.c > help-usage.txt
./help-options-txt.sh ../src/flow/init.c > help-options-base.txt
./help-options-txt.sh ../src/pdes/init.c > help-options-pde.txt
./help-txt.sh extra   ../src/flow/init.c > help-extra.txt

echo "unix man page"
m4 date.m4 > date.yaml
pandoc -s date.yaml feenox.1.md -t man -o feenox.1 \
  --lua-filter=include-files.lua \
  --lua-filter=include-code-files.lua \
  --lua-filter=not-in-format.lua \
  --lua-filter=only-in-format.lua \
  --lua-filter=img-width.lua \
  --lua-filter=manfilter.lua

# this goes into the make script of the webpage
# echo "unix man page in html"
# pandoc -s -t html feenox.1 -o feenox.1.html

# this one goes into the windows zip
pandoc -s double-click.md -t plain -o double-click.txt --lua-filter=include-files.lua



for i in programming compilation srs FAQ CODE_OF_CONDUCT; do
 echo $i
 ./pdf.sh $i
 
  pandoc ${i}.md  -t gfm   -o ${i}.markdown  \
   --standalone --toc --number-sections \
   --reference-links --reference-location=section \
   --lua-filter=include-files.lua \
   --lua-filter=include-code-files.lua \
   --lua-filter=not-in-format.lua \
   --lua-filter=only-in-format.lua \
   --lua-filter=img-width.lua
   
 pandoc ${i}.md  -t plain -o ${i} \
   --standalone --toc --number-sections \
   --reference-links --reference-location=section \
   --lua-filter=include-files.lua \
   --lua-filter=include-code-files.lua \
   --lua-filter=not-in-format.lua \
   --lua-filter=only-in-format.lua \
   --lua-filter=img-width.lua
done

for i in sds feenox-manual; do
 echo $i
 ./pdf.sh $i
done




echo "feenox-desc"
pandoc feenox-desc.md --template template.texi -o feenox-desc.texi \
  --standalone --toc \
  --lua-filter=include-files.lua \
  --lua-filter=include-code-files.lua \
  --lua-filter=not-in-format.lua \
  --lua-filter=only-in-format.lua \
  --lua-filter=img-width.lua

# TODO: as a lua filter
sed -i 's/@verbatim/@smallformat\n@verbatim/' feenox-desc.texi
sed -i 's/@end verbatim/@end verbatim\n@end smallformat/' feenox-desc.texi         

makeinfo feenox-desc.texi > /dev/null
texi2pdf feenox-desc.texi > /dev/null


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


