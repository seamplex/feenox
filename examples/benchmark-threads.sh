#!/bin/bash
set -euo pipefail

usage() {
  cat <<'EOF'
Usage: benchmark-threads.sh INPUT.fee [OUTPUT_DIRECTORY [REPEATS]]

Run CG+GAMG and MUMPS with every combination of OMP_NUM_THREADS and
OPENBLAS_NUM_THREADS in {unset, 1, 2, 4, 8}. Default: 3 repeats (150 runs).
Runs are sequential, with repetitions outside the solver/thread loops.
Run from the same working directory as your usual FeenoX command.

FEENOX selects the executable (default: feenox).
OUTPUT_DIRECTORY must not exist; default: thread-benchmark-YYYYmmdd-HHMMSS.

Outputs:
  summary.csv       Solver, thread settings, repetition, PETSc total seconds,
                    wall/user/system seconds, peak RSS (KiB), exit status.
  *.log             Complete stdout/stderr, including --ksp_view, convergence
                    reasons and --log_view.
  *.petsc.txt       Grepped PETSc totals, stages and all event timing rows.
  *.time.csv        Raw GNU time output.
  metadata.txt      Command, versions, host/CPU and inherited thread settings.

The sweep clears GOTO_NUM_THREADS, OPENBLAS_DEFAULT_NUM_THREADS and
OMP_THREAD_LIMIT so they cannot override the tested settings, and sets
OMP_DYNAMIC=FALSE. "unset" means no requested count, not necessarily 8
threads. Other settings (e.g. affinity and PETSC_OPTIONS) remain inherited.
To measure the libraries' unrestricted defaults for "unset", add
DO_NOT_LIMIT_THREADS to the first PROBLEM instruction in the input file.
Input-file outputs may be overwritten by each run, just as in manual runs.
Failed runs are retained and reported; the script exits nonzero if any fail.
EOF
}

if [[ ${1:-} == -h || ${1:-} == --help ]]; then
  usage
  exit 0
fi
if (( $# < 1 || $# > 3 )); then
  usage >&2
  exit 2
fi

input=$1
output=${2:-thread-benchmark-$(date +%Y%m%d-%H%M%S)}
repeats=${3:-3}
feenox=${FEENOX:-feenox}
if [[ ! -f $input ]]; then
  printf 'Input file not found: %s\n' "$input" >&2
  exit 2
fi
if [[ ! $repeats =~ ^[1-9][0-9]*$ ]]; then
  printf 'REPEATS must be a positive integer.\n' >&2
  exit 2
fi
if ! command -v "$feenox" >/dev/null; then
  printf 'FeenoX executable not found: %s\n' "$feenox" >&2
  exit 2
fi
if [[ ! -x /usr/bin/time ]]; then
  printf 'GNU time is required at /usr/bin/time.\n' >&2
  exit 2
fi
/usr/bin/time --version >/dev/null
mkdir -- "$output"
printf 'For unrestricted "unset" cases, use DO_NOT_LIMIT_THREADS in the first PROBLEM.\n' >&2

{
  date -Is
  printf 'Working directory: %s\n' "$PWD"
  printf 'Executable: %s\nInput: %s\nRepeats: %s\n' "$feenox" "$input" "$repeats"
  printf 'Common options: --ksp_view --ksp_converged_reason --log_view --ksp_error_if_not_converged\n'
  uname -a
  if command -v lscpu >/dev/null; then
    lscpu
  fi
  printf '\nInherited thread settings (before sweep overrides):\n'
  for variable in OMP_NUM_THREADS OMP_THREAD_LIMIT OMP_DYNAMIC OMP_PROC_BIND \
                  OMP_PLACES OMP_WAIT_POLICY OPENBLAS_NUM_THREADS \
                  OPENBLAS_DEFAULT_NUM_THREADS GOTO_NUM_THREADS \
                  MKL_NUM_THREADS BLIS_NUM_THREADS; do
    if [[ -v $variable ]]; then
      printenv "$variable"
    else
      printf 'unset\n'
    fi | sed "s/^/$variable=/"
  done
  printf '\nFeenoX versions:\n'
  "$feenox" --versions
} > "$output/metadata.txt" 2>&1

printf 'solver,omp_threads,openblas_threads,repeat,petsc_seconds,wall_seconds,user_seconds,sys_seconds,max_rss_kib,exit_status\n' \
  > "$output/summary.csv"

failures=0
run=0
total=$((50 * repeats))
for ((repeat = 1; repeat <= repeats; repeat++)); do
  for solver in gamg mumps; do
    if [[ $solver == gamg ]]; then
      options=(--gamg --ksp_type=cg)
    else
      options=(--mumps)
    fi
    for omp in unset 1 2 4 8; do
      for blas in unset 1 2 4 8; do
        run=$((run + 1))
        stem="$output/$solver-omp-$omp-blas-$blas-r$repeat"
        printf '[%d/%d] %s OMP=%s OpenBLAS=%s repeat=%d\n' \
          "$run" "$total" "$solver" "$omp" "$blas" "$repeat"
        status=0
        (
          unset OMP_NUM_THREADS OPENBLAS_NUM_THREADS GOTO_NUM_THREADS \
                OPENBLAS_DEFAULT_NUM_THREADS OMP_THREAD_LIMIT
          export OMP_DYNAMIC=FALSE LC_ALL=C
          if [[ $omp != unset ]]; then
            export OMP_NUM_THREADS=$omp
          fi
          if [[ $blas != unset ]]; then
            export OPENBLAS_NUM_THREADS=$blas
          fi
          exec /usr/bin/time -q -f '%e,%U,%S,%M,%x' -o "$stem.time.csv" \
            "$feenox" "$input" "${options[@]}" --ksp_view \
            --ksp_converged_reason --log_view --ksp_error_if_not_converged
        ) > "$stem.log" 2>&1 || status=$?
        if (( status == 130 || status == 143 )); then
          printf 'Benchmark interrupted; partial results: %s\n' "$output" >&2
          exit "$status"
        fi

        petsc_seconds=$(awk '/^Time \(sec\):/ { print $3; exit }' "$stem.log")
        if ! grep -E \
          'PETSc Performance Summary:|Linear solve (converged|did not converge)|^(Time \(sec\):|Objects:|Flops:|Flops/sec:|MPI |Summary of Stages:|Event[[:space:]]|[[:space:]]+Max Ratio|--- Event Stage)|^[[:space:]]*[0-9]+:|^[[:space:]]*[^[:space:]].*[[:space:]]+[0-9]+[[:space:]]+[0-9]+\.[0-9]+[[:space:]]+[-+0-9.eE]+[[:space:]]' \
          "$stem.log" > "$stem.petsc.txt"; then
          printf 'No PETSc timing/convergence rows found: %s.log\n' "$stem" >&2
        fi
        if (( status != 0 )) || [[ ! $petsc_seconds =~ ^[0-9]+([.][0-9]+)?([eE][-+]?[0-9]+)?$ ]]; then
          failures=$((failures + 1))
          printf 'Failed run (exit=%d or missing PETSc total): %s.log\n' "$status" "$stem" >&2
        fi
        printf '%s,%s,%s,%d,%s,%s\n' "$solver" "$omp" "$blas" "$repeat" \
          "$petsc_seconds" "$(< "$stem.time.csv")" >> "$output/summary.csv"
      done
    done
  done
done

printf 'Results: %s/summary.csv (%d failed runs)\n' "$output" "$failures"
if (( failures > 0 )); then
  exit 1
fi
