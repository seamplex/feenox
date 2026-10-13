#!/bin/sh
for i in . tests; do
  if [ -e ${i}/functions.sh ]; then
    . ${i}/functions.sh
  fi
done
if [ -z "${functions_found}" ]; then
  echo "could not find functions.sh"
  exit 1
fi

gsl_available=no
if ${feenox} --versions | grep 'GSL' | grep -qv 'N/A'; then
  gsl_available=yes
fi

if [ "x${gsl_available}" = "xyes" ]; then
  for name in j0 expint1 expint2 expint3 expintn; do
    case ${name} in
      expintn) expression='expintn(1,1)' ;;
      *) expression="${name}(1)" ;;
    esac
    output=$(printf 'PRINT %s\n' "${expression}" | ${feenox} /dev/stdin 2>&1)
    if [ $? -ne 0 ]; then
      echo "${name}() failed with GSL support: ${output}"
      exit 1
    fi
  done
  exit 0
fi

for name in j0 expint1 expint2 expint3 expintn; do
  case ${name} in
    expintn) expression='expintn(1,1)' ;;
    *) expression="${name}(1)" ;;
  esac
  output=$(printf 'PRINT %s\n' "${expression}" | ${feenox} /dev/stdin 2>&1)
  if [ $? -eq 0 ]; then
    echo "${name}() unexpectedly succeeded without GSL"
    exit 0
  fi
  if ! printf '%s\n' "${output}" | grep -F "error: ${name}() needs FeenoX to be compiled with GSL support" >/dev/null; then
    echo "${name}() returned the wrong error without GSL: ${output}"
    exit 0
  fi
done

echo "GSL-only builtins returned the expected no-GSL errors"
exit 1