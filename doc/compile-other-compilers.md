## Using a different compiler

Without PETSc, FeenoX uses the `CC` environment variable to set the compiler. So configure like

```terminal
./configure CC=clang
```

When PETSc is detected, FeenoX uses `MPICC`. But this variable cannot be set directly. Depending if you are using MPICh or OpenMPI, you should set `MPICH_CC` or `OMPI_CC`:

```terminal
./configure MPICH_CC=icc CC=icc
```

The FeenoX executable will give the configured compiler and flags when invoked with the `--versions` option.
Note that the reported values are the ones used in `configure` and not in `make`. So the recommended way to set flags is in `configure` and not in `make`.
