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

echo "creating SRS and SDS markdown"
cd design
m4 ../header.m4 srs.m4 > ../srs.md
# pandoc srs.md --number-sections --filter pandoc-crossref -o srs.pdf
m4 ../header.m4 sds.m4 > ../sds.md
# pandoc srs.md --number-sections --filter pandoc-crossref -o srs.pdf
cd ..


# exit


echo "creating the reference markdown from the commented sources"
echo " - keywords"
./reference.sh ../src/parser/parser.c              kw > reference-kw.md
echo " - variables"
./reference.sh ../src/flow/init.c                  va > reference-va.md
echo " - functions"
./reference.sh ../src/math/builtin_functions.c     fn > reference-fn.md

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
m4 header.m4 date.m4 feenox.1.md | pandoc -s -t man -o feenox.1

# # manual
# m4 header.m4 feenox.md | \
#   pandoc --toc --template template.texi \
#          --filter pandoc-crossref -o feenox.texi
# sed -i 's/@verbatim/@smallformat\n@verbatim/' feenox.texi
# sed -i 's/@end verbatim/@end verbatim\n@end smallformat/' feenox.texi         
# 

# srs
pandoc srs.md --number-sections --filter pandoc-crossref  -o srs.pdf
