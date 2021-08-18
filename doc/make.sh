#!/bin/bash

for i in touch m4 sed pandoc; do
 if [ -z "$(which $i)" ]; then
  echo "error: $i not installed"
  exit 1
 fi
done

if [ ! -e design ]; then
  echo "error: execute from doc directory"
  exit 1
fi

# main README
echo "creating main README for Github"
cd ..
 # TODO: use include as lua and get rid of m4
 m4 doc/header.m4 README.m4 > README.md
 pandoc README.md  -t gfm   -o README.markdown --standalone --toc --reference-links --reference-location=section --lua-filter=doc/not-in-format.lua --lua-filter=doc/include-code-files.lua
 pandoc README.md  -t plain -o README          --standalone --toc --reference-links --reference-location=section --lua-filter=doc/not-in-format.lua --lua-filter=doc/include-code-files.lua
 pandoc TODO.md    -t plain -o TODO
cd doc
./pdf.sh ../README

# srs & sds
echo "creating doc's README"
pandoc README.md  -t gfm   -o README.markdown --lua-filter=not-in-format.lua --lua-filter=include-code-files.lua --standalone --toc

# srs & sds
echo "creating SRS and SDS markdown"
cd design
m4 ../header.m4 srs.m4 > ../srs.md
m4 ../header.m4 sds.m4 > ../sds.md
cd ..
echo "creating SRS and SDS PDFs"
pandoc -s srs.md --filter pandoc-crossref --lua-filter=not-in-format.lua --lua-filter=include-code-files.lua --pdf-engine=xelatex --number-sections -o srs.pdf
./pdf.sh sds

echo "creating the reference markdown from the commented sources"
echo " - keywords"
./reference.sh ../src/parser/parser.c              kw > reference-kw.md
echo " - variables"
./reference.sh ../src/flow/init.c                  va > reference-va.md
echo " - functions"
./reference.sh ../src/math/builtin_functions.c     fn > reference-fn.md
echo " - functionals"
./reference.sh ../src/math/builtin_functionals.c   fu > reference-fu.md

echo "help as markdown definition list"
./help-md.sh > help.md

echo "help as a raw txt (which is used in feenox -v)"
./help-txt.sh > help.txt

# including a TOC
# touch reference-toc.md
# m4 reference.m4 > reference.md
# pandoc reference.md --toc --template=toc.template -o reference-toc.md


# the reference for the manual is slightly different due to texinfo
# m4 header.m4 reference-manual.m4 > reference-manual.md


echo "unix man page"
m4 header.m4 date.m4 feenox.1.md | pandoc -s -t man --lua-filter=not-in-format.lua --lua-filter=include-code-files.lua --lua-filter=manfilter.lua -o feenox.1

# # manual
# m4 header.m4 feenox.md | \
#   pandoc --toc --template template.texi \
#          --filter pandoc-crossref -o feenox.texi
# sed -i 's/@verbatim/@smallformat\n@verbatim/' feenox.texi
# sed -i 's/@end verbatim/@end verbatim\n@end smallformat/' feenox.texi         
# 
# 
