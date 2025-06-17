#!/bin/bash

# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

usage() {
  cat << EOF
usage: $0 <path_to_md> [ --pdf | --html | --gfm | --plain ] [ --output_dir <dir> ]
 
 The <path_to_md> might or might not include the extension .md.
 Default is to convert to HTML (just because it is faster) and to
 write the converted file in the same directory where the input is.
EOF
  exit 0
}

# check for basic dependencies
for i in getopt dirname basename pandoc pandoc-crossref grep cut locale sed date wc; do
  if [ -z "$(which $i)" ]; then
    echo "error: ${i} not installed"
    exit 1
   fi
done

ARGS=$(getopt -o "h" -l "html,pdf,gfm,plain,tex,output-dir:,help" -n "$0" -- "$@")
# echo $ARGS

if [ $? -ne 0 ]; then
  usage
  exit 1
fi

# defaults
format=".html"
output_dir=""   # this means 

eval set -- "${ARGS}"
while true; do
  case "$1" in
    -h |--help )
      usage
      exit 1;;
      
    --html )
      format=".html"
      shift;;

    --pdf )
      format=".pdf"
      shift;;

    --gfm )
      format=".markdown"
      shift;;
      
    --plain )
      format=""
      shift;;
      
    --tex )
      format=".tex"
      shift;;
      
    --output-dir )
      output_dir="$2"
      shift 2;;

    -- )
      shift
      break;;
      
    * ) break ;;      
  esac
done

shift $((OPTIND-1))
if [ -z "$1" ]; then
   usage
   exit 1
else
  path_to_md="${1}"
fi


# find the root dir
dir=$(dirname ${path_to_md})
cd ${dir}
rootdir=""
for i in . .. ../.. ../../.. ../../../..; do
  if [ -z "${rootdir}" ]; then
    if [ -e ${i}/ax_gcc_builtin.m4 ]; then
      rootdir=${i} 
    fi
  fi 
done

if [ -z "${rootdir}" ]; then
 echo "error: cannot find root dir"
 exit 1
fi


# ----------------------------------------------------------------
currentdateedtf=$(date +%Y-%m-%d)

headepoch=$(git log -1 --format="%ct")
headdateedtf=$(date -d@${headepoch} +%Y-%m-%d)

hash=$(git rev-parse --short HEAD)

# if the current working tree is not clean, we add a "+"
# (should be $\epsilon$, but it would screw the file name),
# set the date to today and change the author to the current user,
# as it is the most probable scenario
if [ -z "$(git status --porcelain)" ]; then
  dateedtf=${headdateedtf}
else
  dateedtf=${currentdateedtf}
  hash="${hash}+dirty"
fi

cat << EOF > hash.yaml
---
hash: ${hash}
date: ${dateedtf}
...
EOF
# ----------------------------------------------------------------

# check output dir
in=$(basename ${path_to_md} .md)
if [ -z "${output_dir}" ]; then
  out="${in}${format}"
  output_dir_to_print="${dir}"
else
  if [ ! -f "${output_dir}" ]; then
    echo "error: directory ${output_dir} does not exist"
    exit 1
  fi
  out="${output_dir}/${in}${format}"
  output_dir_to_print="${output_dir}"
fi

# check path to markdown
if [ ! -f ${in}.md ]; then
  echo "error: cannot find ${in}.md"
  exit 1
fi

# we're good to go
echo "${dir}/${in}.md -> ${output_dir_to_print}/${out}"

# default template name for html and pdf
template=$(grep template: ${in}.md | cut -d' ' -f2)
if [ -z "${template}" ]; then
  template="template"
fi


if [ "x${format}" = "x.html" ]; then
  format_args="--katex --email-obfuscation=javascript -t html5+smart"
  template_args="--template=${rootdir}/doc/${template}.html"
  if [ -e ${in}.pdf ]; then
    format_args+=" --metadata=pdf_available:${in}"
  fi
 
elif [ "x${format}" = "x.markdown" ]; then
  format_args="--reference-links --reference-location=section -t gfm"
  
elif [ "x${format}" = "x" ]; then
  format_args="--reference-links --reference-location=section -t plain"

elif [ "x${format}" = "x.tex" ]; then
  pdfyaml=$(grep pdfyaml: ${in}.md | cut -d' ' -f2)
  if [ -z "${pdfyaml}" ]; then
    pdfyaml="pdf.yaml"
  fi
  
  format_args="--pdf-engine=xelatex --listings --metadata=listings:true -H ${rootdir}/doc/syntax-feenox.tex -H ${rootdir}/doc/syntax.tex ${rootdir}/doc/${pdfyaml}"
#   template_args="--template=${rootdir}/doc/${template}.tex"
  
elif [ "x${format}" = "x.pdf" ]; then

  # check for xelatex
  if [ -z "$(which xelatex)" ]; then
    echo "error: PDF output needs XeLaTeX"
    exit 1
  fi
  
  pdfyaml=$(grep pdfyaml: ${in}.md | cut -d' ' -f2)
  if [ -z "${pdfyaml}" ]; then
    pdfyaml="pdf.yaml"
  fi
  
  format_args="--pdf-engine=xelatex --listings --metadata=listings:true -H ${rootdir}/doc/crossref.tex -H ${rootdir}/doc/syntax-feenox.tex -H ${rootdir}/doc/syntax.tex ${rootdir}/doc/${pdfyaml}"
#   template_args="--template=${rootdir}/doc/${template}.tex"
fi


if [ "x$(grep toc: ${in}.md)" = "xfalse" ]; then
  toc_option=""
else
  toc_option="--toc"
fi

# TODO: check if filters, templates, etc. exist

# here's the meat
pandoc hash.yaml ${in}.md \
    --quiet \
    --standalone ${toc_option} \
    --metadata=rootdir:${rootdir} \
    --syntax-definition=${rootdir}/doc/feenox.xml \
    --lua-filter=${rootdir}/doc/include-files.lua \
    --lua-filter=${rootdir}/doc/include-code-files.lua \
    --lua-filter=${rootdir}/doc/code-style.lua \
    --lua-filter=${rootdir}/doc/not-in-format.lua \
    --lua-filter=${rootdir}/doc/only-in-format.lua \
    --lua-filter=${rootdir}/doc/img-width.lua \
    --filter pandoc-crossref \
    --number-sections --toc-depth=4 \
    ${template_args} ${format_args} -o ${out}

if [ $? -ne 0 ]; then
  echo "error: pandoc failed"
  exit 1
fi

# post
if [ "x${format}" = "x.html" ]; then
  n1=$(grep -n side_toc_start ${out} | cut -d':' -f1)
  n2=$(grep -n side_toc_end   ${out} | cut -d':' -f1)

  if [ ! -z "${n1}" ]; then
    # set list-unstyled
    sed -i "${n1},${n2} s/<ul>/<ul class=\"list-unstyled\">/"   ${out}

    # replace <li> with <li class="nav-item">
    sed -i "${n1},${n2} s/<li>/<li class=\"nav-item\">/"        ${out}

    # replace <a> width <a class="nav-link scrollto">
    sed -i "${n1},${n2} s/<a/<a class=\"nav-link scrollto\"/" ${out}
  fi

  # sections
  sed -i 's/<h1 data-number/<h1 class="section-title" data-number/' ${out}
  # sed -i 's/<h2 data-number/<h2 class="section-title" data-number/' ${out}

  # bootstrap tables
  sed -i 's/<table>/<table class=\"table table-striped table-hover table-sm table-responsive-sm\">/'  ${out} 

  # bootstrap/prettydocs blockquotes
  sed -i 's/<blockquote>/<blockquote class=\"callout-block callout-blockquote\">/'     ${out} 

  # img-fluid
  sed -i 's/<img /<img class=\"img-fluid\" /'     ${out} 

  # update links (if the link starts with ./file.md it gets translated to ./file.html otherwise it remains .md)
  sed -i 's/\(.*\)\.md/\1.html/g'  ${out}
  
elif [ "x${format}" = "x.markdown" ]; then
  sed -i 's/.\/\(.*\)\.md/\1.markdown/g'  ${out}
  sed -i '/hash:/d' ${out}
  
fi
 
