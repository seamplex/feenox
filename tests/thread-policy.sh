#!/bin/sh
set -eu

for i in . tests; do
  if [ -e "$i/functions.sh" ]; then
    . "$i/functions.sh"
  fi
done
if [ -z "${functions_found:-}" ]; then
  echo "could not find functions.sh" >&2
  exit 1
fi
checkpde thermal

if [ "$(uname -s)" != Linux ]; then
  echo "LD_PRELOAD probe requires Linux"
  exit 77
fi

tmp=$(mktemp -d)
trap 'rm -f "$tmp/probe.c" "$tmp/probe.so"; rmdir "$tmp"' EXIT HUP INT TERM
cat > "$tmp/probe.c" <<'EOF'
#include <stdio.h>
#include <stdlib.h>

void openblas_set_num_threads(int n) { fprintf(stderr, "thread-policy blas %d\n", n); }
void omp_set_num_threads(int n) { fprintf(stderr, "thread-policy omp %d\n", n); }
int openblas_get_parallel(void) {
  const char *value = getenv("TEST_OPENBLAS_PARALLEL");
  return value == NULL ? 1 : atoi(value);
}
EOF
${CC:-cc} -shared -fPIC -o "$tmp/probe.so" "$tmp/probe.c"

run_case() {
  expected=$1
  input=$2
  shift 2
  if ! output=$(printf '%b\n' "$input" | env \
    -u OMP_NUM_THREADS -u OMP_THREAD_LIMIT -u OPENBLAS_NUM_THREADS \
    -u OPENBLAS_DEFAULT_NUM_THREADS -u GOTO_NUM_THREADS -u TEST_OPENBLAS_PARALLEL \
    "$@" LD_PRELOAD="$tmp/probe.so" "$feenox" - --check 2>&1); then
    printf 'FeenoX failed for %s:\n%s\n' "$input" "$output" >&2
    exit 1
  fi
  actual=$(printf '%s\n' "$output" | grep '^thread-policy ' || :)
  if [ "$actual" != "$expected" ]; then
    printf 'For %s, expected:\n%s\ngot:\n%s\n' "$input" "$expected" "$actual" >&2
    exit 1
  fi
}

both='thread-policy blas 1
thread-policy omp 1'
run_case "$both" 'PROBLEM thermal 1D'
run_case "$both" 'PROBLEM thermal 1D LIMIT_THREADS LINEAR'
run_case '' 'PROBLEM thermal 1D DO_NOT_LIMIT_THREADS LINEAR'
run_case "$both" 'PROBLEM thermal 1D DO_NOT_LIMIT_THREADS LIMIT_THREADS'
run_case '' 'PROBLEM thermal 1D LIMIT_THREADS DO_NOT_LIMIT_THREADS'
run_case '' 'PRINT 2'
run_case '' 'PROBLEM thermal 1D' OMP_NUM_THREADS=4
run_case '' 'PROBLEM thermal 1D' OMP_THREAD_LIMIT=4
run_case 'thread-policy omp 1' 'PROBLEM thermal 1D' OPENBLAS_NUM_THREADS=4
run_case 'thread-policy omp 1' 'PROBLEM thermal 1D' OPENBLAS_DEFAULT_NUM_THREADS=4
run_case 'thread-policy omp 1' 'PROBLEM thermal 1D' GOTO_NUM_THREADS=4
run_case '' 'PROBLEM thermal 1D' TEST_OPENBLAS_PARALLEL=2 OPENBLAS_DEFAULT_NUM_THREADS=4
run_case '' 'PROBLEM thermal 1D' TEST_OPENBLAS_PARALLEL=-1 OPENBLAS_DEFAULT_NUM_THREADS=4
run_case '' 'PROBLEM thermal 1D' OMP_NUM_THREADS=4 OPENBLAS_NUM_THREADS=2
run_case "$both" 'PROBLEM thermal 1D\nPROBLEM LINEAR'

if printf 'PROBLEM thermal 1D LIMIT_THREADS no\n' | "$feenox" - --check >/dev/null 2>&1; then
  echo "obsolete LIMIT_THREADS argument accepted" >&2
  exit 1
fi
echo "thread policy: ok"
