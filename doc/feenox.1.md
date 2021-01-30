---
title: FEENOX(1) FeenoX | FeenoX User Manual
author: Jeremy Theler <jeremy@seamplex.com>
... 
  
# NAME

FeenoX - a free no-fee no-X uniX-like finite-element(ish) computational engineering tool

# SYNOPSIS

The basic usage is to execute the **feenox** binary passing a path to an input file that defines the problem, along with other options and command-line replacement arguments which are [explained below](#options):

**feenox** [_options_ ...] _input-file_ [_optional_commandline_replacement_arguments_ ...]

For large problems that do not fit in a single computer, a parallel run using **mpirun**`(1)` will be needed:

**mpirun** **-n** _number_of_threads_ **feenox** [_options_ ...] _input-file_ [_optional_commandline_replacement_arguments_ ...]


Besides executing the **feenox** binary, there is an API that allows to use FeenoX from Python or Julia.


# DESCRIPTION

FeenoX is a computational tool that can solve engineering problems which are usually casted as differential-algebraic equations (DAEs) or partial differential equations (PDEs). In particular, it can solve

 * dynamical systems defined by a set of user-provided DAEs (such as plant control dynamics for example)
 * mechanical elasticity
 * heat conduction
 * structural modal analysis
 * neutron diffusion

FeenoX reads a plain-text input file which contains the problem definition and writes 100%-user defined results in ASCII (through *PRINT* or other user-defined output instructions within the input file). For PDE problems, it needs a reference to at least one **gmsh**`(1)` mesh file for the discretization of the domain. It can write post-processing views in either _.msh_ or _.vtk_ formats. 

Keep in mind that FeenoX is just a back end reading a set of input files and writing a set of output files following the design philosophy of UNIX (separation, composition, representation, economy, extensibility, etc). Think of it as a transfer function between input files and output files:

```
                  +----------------+
                  |                |
input files ----->|     FeenoX     |-----> output files
                  |                |
                  +----------------+ 
```

Following the UNIX programming philosophy, there are no graphical interfaces attached to the FeenoX core, although a wide variety of pre and post-processors can be used with FeenoX. See for example <https://www.caeplex.com> for a web-based interface.


# OPTIONS

include(help.md)


# EXAMPLES

## The Lorenz system

## Tensile test minimum working example

The following is a MWE input file for FeenoX that reads a Gmsh-generated _.msh_ file, solves a linear elastic problem and writes the results in a *.vtk* file which can be post-processed by **paraview**`(1)`:

```
include(../examples/tensile-mwe.fee)
```

The _.geo_ file that generates the mesh with Gmsh and the CAD file in _.step_ format can be found in the _examples_ directory.

## Extended annotated example

The example above can be extended to give more information as the following annotated input shows:

```
include(../examples/tensile-test.fee)
```

 

# EXIT STATUS


# ENVIRONMENT


# FILES


# CONFORMING TO


# INPUT-FILE KEYWORDS

include(reference-kw.md)

# SPECIAL VARIABLES

TBD.

# MATERIAL PROPERTIES

TBD.

# BOUNDARY CONDITIONS

TBD.

# RESULTING DISTRIBUTIONS

TBD.

# BUILT-IN FUNCTIONS

TBD.

# BUILT-IN FUNCTIONALS

TBD.

# BUILT-IN VECTOR FUNCTIONS

TBD.





# NOTES

TBD.

# BUGS

Report on Github or at <jeremy@seamplex.com>

# SEE ALSO

**gmsh**`(1)`, **mpirun**`(1)`, **paraview**`(1)`

The FeenoX web page contains links to the full source code, binary versions, updates, examples, verification & validation cases and full documentation:
<https://www.seamplex.com/feenox>.

The FeenoX Case files contains fully-annotated example:
<https://www.seamplex.com/feenox/cases/>
