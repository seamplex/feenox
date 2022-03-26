#!/bin/sh
# take the output of this script as a vim syntax definition file for FeenoX
#
# mkdir -p $HOME/.vim/syntax/
# ./syntax-vim.sh > $HOME/.vim/syntax/fee.vim
#

. ./keywords.sh

cat << EOF
" Vim syntax file
" Language: FeenoX input file
" Maintainer: jeremy Theler
" Latest Revision: 19 March 2022

if exists("b:current_syntax")
  finish
endif

EOF

# primary keywords
echo -n "syn keyword feeKeyword"
for kw in ${UPPER}; do
  echo -n " ${kw}"
done
echo


# secondary keywords (TO-DO)
# echo "    <list name=\"sec_keywords\">"
# echo "    </list>"


# special variables
echo -n "syn keyword feeVariable"
for kw in ${VARS}; do
  echo -n " ${kw}"
done
echo

# functions
echo -n "syn keyword feeFunction"
for kw in ${FUNCS}; do
  echo -n " ${kw}"
done
echo


cat << EOF
" Numbers, allowing signs (both -, and +)
" Integer number.
syn match  feeNumber		display "[+-]\\=\\<\\d\\+\\>"
" Floating point number.
syn match  feeFloat		display "[+-]\\=\\<\\d\\+\\.\\d+\\>"
" Floating point number, starting with a dot.
syn match  feeFloat		display "[+-]\\=\\<.\\d+\\>"
syn case ignore
"floating point number, with dot, optional exponent
syn match  feeFloat	display "\\<\\d\\+\\.\\d*\\(e[-+]\\=\\d\\+\\)\\=\\>"
"floating point number, starting with a dot, optional exponent
syn match  feeFloat	display "\\.\\d\\+\\(e[-+]\\=\\d\\+\\)\\=\\>"
"floating point number, without dot, with exponent
syn match  feeFloat	display "\\<\\d\\+e[-+]\\=\\d\\+\\>"
syn case match


syn match  feeComment     "#.*"

" Expression separators: ';' and ','
syn match  feeSemicolon   ";"
syn match  feeComma       ","
syn match  feeLineSkip    "\\\\$"

syn match  feeOperator     /+\\||\\|\\.\\.\\|-\\|(\\|)\\|{\\|}\\|\\[\\|\\]\\|=\\|<\\|>/



" Define the default highlighting.
hi def link feeKeyword     Statement
hi def link feeFunction    Function
hi def link feeVariable    Identifier
hi def link feeComment     Comment
hi def link feeSemicolon   Special
hi def link feeComma       Special
hi def link feeLineSkip    Special
hi def link feeOperator    Operator
hi def link feeNumber      Float
hi def link feeFloat       Float

let b:current_syntax = "fee"
EOF
