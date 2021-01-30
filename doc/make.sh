#!/bin/bash

for i in touch m4 pandoc; do
 if [ -z "$(which $i)" ]; then
  echo "error: $i not installed"
  exit 1
 fi
done


# create the reference markdown from the commented sources
./reference.sh ../src/parser/parser.c kw > reference-kw.md
./reference.sh ../src/flow/init.c     va > reference-va.md


# help as markdown definition list
./help-md.sh > help.md

# help as a raw txt (which is used in feenox -v)
./help-txt.sh > help.txt


# including a TOC
# touch reference-toc.md
# m4 reference.m4 > reference.md
# pandoc reference.md --toc --template=toc.template -o reference-toc.md


# the reference for the manual is slightly different due to texinfo
# m4 header.m4 reference-manual.m4 > reference-manual.md


# unix man page
m4 header.m4 date.m4 feenox.1.md | pandoc -s -t man -o feenox.1

# manual
m4 header.m4 feenox.md | \
  pandoc --toc --template template.texi \
         --filter pandoc-crossref -o feenox.texi
sed -i 's/@verbatim/@smallformat\n@verbatim/' feenox.texi
sed -i 's/@end verbatim/@end verbatim\n@end smallformat/' feenox.texi         

