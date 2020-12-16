% FEENOX(1) FeenoX User Manual
% Jeremy Theler

# NAME

feenox - a free finite-element thermo-mechanical solver

# SYNOPSIS

feenox [*options*] input-file [*optional_extra_arguments*]...


# DESCRIPTION

FeenoX is a free and open source tool released under the terms of the GPLv3+ that uses the finite-element method to solve

 * steady or quasistatic thermo-mechanical problems, or
 * steady or transient heat conduction problems, or
 * modal analysis problems.

FeenoX reads a plain-text input file which contains the problem definition and a reference to a Gmsh(1) mesh file and it writes 100%-user defined results in ASCII (through `PRINT` instructions within the input file) and/or post-processing views in either `.msh` or `.vtk` formats.

 
# EXAMPLES

## Minimum working example

The following is a MWE input file for FeenoX that reads a Gmsh-generated `.msh` file, solves a linear elastic problem and wriets the results in a `.vtk` file which can be post-processed by Paraview:

```
include(../examples/tensile-mwe.fin)
```

The `.geo` file that generates the mesh with Gmsh and the CAD file in `.step` format can be found in the `examples` directory.

## Extended annotated example

The example above can be extended to give more information as the following annotated input shows:

```
include(../examples/tensile-test.fin)
```

 
# OPTIONS

include(help.md)

# REFERENCE

## Keywords

TBD.

## Special input distributions

TBD.


## Boundary conditions

TBD.


## Result functions

TBD.


## Special variables

TBD.


# SEE ALSO

`gmsh`(1), `paraview`(1)

The FeenoX Case files at <https://www.seamplex.com/feenox/cases/> contains fully-discussed examples.

The FeenoX web page contains full source code, updates, examples, V&V cases and full reference:
<https://www.seamplex.com/feenox>.

# AUTHOR

Jeremy Theler <https://www.seamplex.com>
