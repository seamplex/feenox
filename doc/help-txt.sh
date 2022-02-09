#!/bin/bash -e

if [ -z "${2}" ]; then
  echo "usage: ${0} filter source.c"
  exit 1
fi

grep "///help+${1}+desc" ${2} | cut -d" " -f2- | sed 's/@$//' | sed 's_/\\/_//_' | pandoc -t plain
