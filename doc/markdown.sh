#!/bin/bash -e
if [ -z "$1" ]; then
  echo "usage: $0 what"
  exit
else
  stuff=$1
fi

if [ ! -e "${stuff}.md" ]; then
  stuff=$(basename ${stuff} .md)
  if [ ! -e "${stuff}.md" ]; then
    echo "cannot find ${stuff}.md"
    exit 1
  fi  
fi

# check for needed tools
for i in pandoc; do
 if [ -z "$(which $i)" ]; then
  echo "error: $i not installed"
  exit 1
 fi
done


dir=""
for i in . .. doc ../..; do
  if [ -e ${i}/code-style.lua ]; then
    dir=${i}
  fi
done
if [ -z "${dir}" ]; then
 echo "cannot find directory with Lua filters"
fi

if [ "x$(grep toc: ${stuff}.md)" = "xfalse" ]; then
  toc=""
else
  toc="--toc"
fi


pandoc ${stuff}.md -f markdown+emoji -t gfm   -o ${stuff}.markdown  \
  --standalone ${toc} --number-sections \
   --reference-links --reference-location=section \
  --lua-filter=${dir}/include-files.lua \
  --lua-filter=${dir}/include-code-files.lua \
  --lua-filter=${dir}/not-in-format.lua \
  --lua-filter=${dir}/only-in-format.lua \
  --lua-filter=${dir}/code-style.lua \
  --lua-filter=${dir}/img-width.lua

pandoc ${stuff}.md -f markdown+emoji -t plain   -o ${stuff}  \
  --standalone ${toc} --number-sections \
   --reference-links --reference-location=section \
  --lua-filter=${dir}/include-files.lua \
  --lua-filter=${dir}/include-code-files.lua \
  --lua-filter=${dir}/not-in-format.lua \
  --lua-filter=${dir}/only-in-format.lua \
  --lua-filter=${dir}/code-style.lua \
  --lua-filter=${dir}/img-width.lua
  
