#!/bin/bash
if [ -z "$1" ]; then
  echo "usage: $0 what"
  exit
else
  stuff=$1
fi

# check for needed tools
for i in grep cut locale sed date wc; do
 if [ -z "$(which $i)" ]; then
  echo "error: $i not installed"
  exit 1
 fi
done

if [ ! -e "${stuff}.md" ]; then
  stuff=$(basename ${stuff} .md)
  if [ ! -e "${stuff}.md" ]; then
    echo "cannot find ${stuff}.md"
    exit 1
  fi  
fi


# get the date right
lang=$(grep lang: ${stuff}.md | cut -d" " -f2 | sed 's/-/_/')
if [ -z "${lang}" ]; then
  lang="en_US"
fi
language="${lang}.utf8"
LC=$(locale -a | grep ${language} | head -n1)
if [ -z "$LC" ]; then
  echo "error: locale for language ${language} is not installed, run"
  echo "# dpkg-reconfigure locales"
  echo "or equivalent (or change the document's language)"
  exit 1
fi

# see how we are supposed to write the date, but
# replace %m with %b to get a string for the month
dateformat=$(LC_ALL=${LC} locale d_fmt | sed s/m/b/)
currentdate=$(LC_ALL=${LC} date +${dateformat})
currentdateedtf=$(LC_ALL=${LC} date +%Y-%m-%d)

branch=$(git rev-parse --abbrev-ref HEAD)
dochash=$(git rev-parse --short HEAD)

if [ -e "baserev" ]; then
  basehash=$(cat baserev)
else
  basehash=$(git rev-parse HEAD)
fi

headepoch=$(git log -1 --format="%ct")
headdate=$(LC_ALL=${DATELC} date -d@${headepoch} +${dateformat})
headdateedtf=$(LC_ALL=${DATELC} date -d@${headepoch} +%Y-%m-%d)

tag=$(git tag --sort=-version:refname | head -n1)

if [ -z "${tag}" ]; then
  revision="DRAFT"
  docver=$(git rev-list ${basehash}..HEAD | wc -l)
else
  revision=${tag}
  docver=$(git rev-list ${tag}..HEAD | wc -l)
fi

if [ ${docver} -ne 0 ]; then
  revision="${revision}${docver}"
fi

hash=$(git rev-parse --short HEAD)


# if the current working tree is not clean, we add a "+"
# (should be $\epsilon$, but it would screw the file name),
# set the date to today and change the author to the current user,
# as it is the most probable scenario
if [ -z "$(git status --porcelain)" ]; then
  date=${headdate}
  dateedtf=${headdateedtf}
else
  dochash="${hash}+\$\\epsilon\$"
  revision="${revision}+"
  date=${currentdate}
  dateedtf=${currentdateedtf}
fi




cat << EOF > hash.yaml

---
hash: ${dochash}
rev: ${revision}
date: ${date}
...
EOF

dir=""
for i in . .. doc; do
  if [ -e ${i}/code-style.lua ]; then
    dir=${i}
  fi
done
if [ -z "${dir}" ]; then
 echo "cannot find directory with TeX template and Lua filters"
fi


pandoc hash.yaml ${stuff}.md \
  --standalone --toc \
  --filter pandoc-crossref \
  --lua-filter=${dir}/include-files.lua \
  --lua-filter=${dir}/include-code-files.lua \
  --lua-filter=${dir}/not-in-format.lua \
  --lua-filter ${dir}/code-style.lua \
  --template=${dir}/single.tex \
  --pdf-engine=xelatex \
  --listings --number-sections \
  -o ${stuff}.pdf

