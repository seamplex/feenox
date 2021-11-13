#!/bin/bash

if [[ -z "${1}" ]]; then
  echo "usage: $0 user@host [key]"
  exit 1
fi

if [[ -z "${2}" ]]; then
  key=""
else
  key="-i ${2}"
fi

scp ${key} ${1}:feenox/tests/nafems/le10/*.dat .
scp ${key} ${1}:feenox/tests/nafems/le10/arch*.md .
scp ${key} ${1}:feenox/tests/nafems/le10/report*.md .
scp ${key} ${1}:feenox/tests/nafems/le10/version*.txt .
scp ${key} ${1}:feenox/tests/nafems/le10/le10-ref.txt .
scp ${key} ${1}:feenox/tests/nafems/le10/*.svg .
