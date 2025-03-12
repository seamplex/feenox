# Tensile-test case example for `fee2ccx`

Say one has a nice syntactically-sugared human-friendly English-like FeenoX input file with a one-to-one correspondence between the computer and the human formulation of a the tensile test speciment from [Tutorial #1](https://www.seamplex.com/feenox/doc/tutorials/110-tensile-test/) named [`tensile-test.fee`](tensile-test.fee):

```feenox
PROBLEM mechanical           # self-descriptive
READ_MESH tensile-test.msh   # lengths are in mm

# material properties, E and nu are "special" variables for the mechanical problem
E = 200e3   # [ MPa = N / mm^2 ]
nu = 0.3

# boundary conditions, fixed and Fx are "special" keywords for the mechanical problem
# the names "left" and "right" should match the physcal names in the .geo
BC left  fixed
BC right Fx=10e3  # [ N ]

# we can now solve the problem, after this keyword the results will be available for output
SOLVE_PROBLEM

# guess
WRITE_RESULTS FORMAT vtk
```

![](gmsh-mesh-surface-only.png)


## Conversion of the input from `.fee` to `.inp`

We can convert it to the non-human-friendly `.inp` format with `fee2ccx` (we have to make sure the mesh exists):

```terminal
gmsh  -3 tensile-test.geo
fee2ccx tensile-test.fee > tensile-test.inp
```


## Solution of the `.inp` with `ccx`

We can then solve the `.inp` with CalculiX

```terminal
ccx -i tensile-test
```

Just to compare, let's also solve the same problem with FeenoX (with some timing to have some fun):

```terminal
$ time feenox tensile-test.fee

real    0m2.135s
user    0m1.814s
sys     0m0.056s
$ time feenox tensile-test.fee --mumps

real    0m1.109s
user    0m0.757s
sys     0m0.085s
$ time ccx -i tensile-test
[...]
[ASCII art and non-Unix-friendly output]

Job finished

________________________________________

Total CalculiX Time: 1.458292
________________________________________

real    0m1.479s
user    0m1.409s
sys     0m0.064s
$ 
```


## Conversion of the output from `.frd` to `.vtk`

CalculiX spits out (among a few other files that seem from 1969) a file ending in `.frd` (which also looks from 1969) that contains the post-processing views.
This file can be converted to VTK using this Python script: <https://github.com/calculix/ccx2paraview> (there might be others as well, but this comes from the organization `calculix`).

Or, we can use FeenoX to read the `.frd` using `READ_MESH` and then write it with `WRITE_MESH` in either `.msh` or `.vtk`:

```feenox
READ_MESH $1.frd DIM 3 {
 READ_FUNCTION D1
 READ_FUNCTION D2
 READ_FUNCTION D3
 READ_FUNCTION EXX 
 READ_FUNCTION EYY
 READ_FUNCTION EZZ 
 READ_FUNCTION EXY 
 READ_FUNCTION EYZ 
 READ_FUNCTION EZX
 READ_FUNCTION SXX 
 READ_FUNCTION SYY
 READ_FUNCTION SZZ 
 READ_FUNCTION SXY 
 READ_FUNCTION SYZ 
 READ_FUNCTION SZX
}

WRITE_MESH $1.vtk {
 VECTOR D1 D2 D3
 EXX EYY EZZ EXY EYZ EZX
 SXX SYY SZZ SXY SYZ SZX
}
```

Calling this through FeenoX, converts `tensile-test.frd` to `tensile-test.frd.vtk` (we could have also converted to `.msh`) so it can be post-processed with Paraview:

```terminal
feenox frd2vtk.fee tensile-test.frd
```


## Comparison with FeenoX

Even more, we FeenoX can perform algebraic operations on the read data.
In particular, we can compute errors:

```feenox
READ_MESH $1.vtk DIM 3 { 
  READ_FUNCTION displacements1
  READ_FUNCTION displacements2
  READ_FUNCTION displacements3
  READ_FUNCTION sigmax
  READ_FUNCTION sigmay
  READ_FUNCTION sigmaz
  READ_FUNCTION tauxy
  READ_FUNCTION tauyz
  READ_FUNCTION tauzx
}

READ_MESH $1.frd DIM 3 {
  READ_FUNCTION D1
  READ_FUNCTION D2
  READ_FUNCTION D3
  READ_FUNCTION SXX
  READ_FUNCTION SYY
  READ_FUNCTION SZZ
  READ_FUNCTION SXY
  READ_FUNCTION SYZ
  READ_FUNCTION SZX
}

du(x,y,z) = displacements1(x,y,z) - D1(x,y,z)
dy(x,y,z) = displacements2(x,y,z) - D2(x,y,z)
dz(x,y,z) = displacements3(x,y,z) - D3(x,y,z)

dsigmax = sigmax(x,y,z) - SXX(x,y,z)
dsigmay = sigmay(x,y,z) - SYY(x,y,z)
dsigmaz = sigmaz(x,y,z) - SZZ(x,y,z)
dtauxy = tauxy(x,y,z) - SXY(x,y,z)
dtauyz = tauyz(x,y,z) - SYZ(x,y,z)
dtauzx = tauzx(x,y,z) - SZX(x,y,z)

WRITE_MESH $1-diff.vtk VECTOR du dy dz dsigmax dsigmay dsigmaz dtauxy dtauyz dtauzx

INTEGRATE du(x,y,z)^2+dy(x,y,z)^2+dz(x,y,z)^2 RESULT e2
PRINT e2
```


```terminal
$ feenox diff.fee tensile-test
1.11059e-08
$ 
```
