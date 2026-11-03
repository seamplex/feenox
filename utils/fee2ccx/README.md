# FeenoX to CalculiX input file converter

Converts an input file in FeenoX's `.fee` format (with `PROBLEM mechanical`) to CalculiX's `.inp` format.

Say one has a nice syntactically-sugared human-friendly English-like FeenoX input file with a one-to-one correspondence between the computer and the human formulation of a the tensile test speciment from [Tutorial #1](https://www.seamplex.com/feenox/doc/tutorials/110-tensile-test/) (see directory [`examples/tensile-test`](examples/tensile-test):

```fee
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
```

![](gmsh-mesh-surface-only.png)

We can convert it to the non-human-friendly `.inp` format with `fee2ccx` (we have to make sure the mesh exists):

```terminal
gmsh  -3 tensile-test.geo
fee2ccx tensile-test.fee > tensile-test.inp
# or
fee2ccx -o tensile-test.inp tensile-test.fee
```

and then solve it with

```
ccx -i tensile-test
```

In particular, `fee2ccx` 

 * reads the mesh specified in the main `READ_MESH` instruction and creates `*NODE` and `*ELEMENT` sections in the `.inp`.
 * evaluates the material properties and creates a `*MATERIAL` section (note that non-uniform material properties are not easily handled in CalculiX, help is welcome here)
 * evaluates the Dirichlet boundary conditions in the `.fee` and creates a `*BOUNDARY` section (node by node)
 * evaluates the Neumann boundary conditions and volumetric sources in the `.fee` and creates a `*CLOAD` section (node by node)
 * converts a homogeneous linear-isotropic material to a CalculiX `*ELASTIC` card

Run `fee2ccx --help` for command-line options. The converter validates the input model and rejects unsupported cases rather than silently writing a different model.

## Example: NAFEMS LE10 Benchmark problem


> **Homework** Solve the original problem with
>
> ```terminal
> feenox tensile-test.fee
> ```
>
> and compare
>
>  1. Results
>  2. Solution Times


## Limitations

 * Homogeneous isotropic linear elasticity only; material values are sampled once during conversion
 * Single-step, small-strain static problems only; transient, multi-step quasi-static, and geometrically/materially nonlinear analyses are rejected
 * Serial execution only
 * 3D supported elements are limited to those present in the FeenoX-to-CalculiX element map
 * 2D triangle3/triangle6 and quadrangle4/quadrangle8 elements are supported for plane stress, plane strain, and axisymmetric formulations; quadrangle9 is rejected because there is no equivalent mapped element in this converter
 * Boundary and body loads are exported as the assembled nodal equivalent force vector (`*CLOAD`). This is suitable for the supported static linear case but does not preserve the original surface/body load definition.
 * Thermal expansion, multiple materials, and spatially varying material properties are unsupported and rejected. Time-dependent function/property material definitions are rejected; scalar material variables are exported at their value during conversion.

## TODOs

 * orthotropic materials (might need some re-thinking on FeenoX's side)
 * multi-volume material properties
 * non-uniform material properties
 * quasi-static
