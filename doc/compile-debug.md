## Adding debug symbols

By default the C flags are `-O3`, without debugging. To add the `-g` flag, just use `CFLAGS` when configuring:

```
./configure CFLAGS="-g -O0"
```


## Using a different compiler

Without PETSc, FeenoX uses the `CC` environment variable to set the compiler. So configure like

```
./configure CC=clang
```

When PETSc is detected, FeenoX uses `MPICC`. But this variable cannot be set directly. Depending if you are using MPICh or OpenMPI, you should set `MPICH_CC` or `OMPI_CC`:

```
./configure MPICH_CC=icc CC=icc
```
