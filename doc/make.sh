#!/bin/bash

for i in m4 pandoc; do
 if [ -z "$(which $i)" ]; then
  echo "error: $i not installed"
  exit 1
 fi
done

# manual
m4 header.m4 feenox.md | \
  pandoc --toc --template template.texi \
         --filter pandoc-crossref -o feenox.texi
sed -i 's/@verbatim/@smallformat\n@verbatim/' feenox.texi
sed -i 's/@end verbatim/@end verbatim\n@end smallformat/' feenox.texi         

