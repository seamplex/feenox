#!/bin/bash

if [[ -z "${1}" ]]; then
  echo "usage: $0 user@host [key]"
  exit 1
fi

if [[ -z "${2}" ]]; then
  scp ${1}:feenox/tests/nafems/le10/*.dat .
  scp ${1}:feenox/tests/nafems/le10/arch*.md .
  scp ${1}:feenox/tests/nafems/le10/report*.md .
  scp ${1}:feenox/tests/nafems/le10/version*.txt .
else
  scp -i ${2} ${1}:feenox/tests/nafems/le10/*.dat .
  scp -i ${2} ${1}:feenox/tests/nafems/le10/arch*.md .
  scp -i ${2} ${1}:feenox/tests/nafems/le10/report*.md .
  scp -i ${2} ${1}:feenox/tests/nafems/le10/version*.txt .
fi
