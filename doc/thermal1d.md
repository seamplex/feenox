One of the main features of this allegedly particular design basis is that **simple problems ought to have simple inputs** (_rule of simplicity_) or, quoting Alan Kay, “simple things should be simple, complex things should be possible.”
For instance, to solve one-dimensional heat conduction over the domain $x\in[0,1]$ (which is indeed one of the most simple engineering problems we can find) the following input file is enough:

```{.feenox include="thermal-1d-dirichlet-constant-k.fee"}
```

```terminal
$ feenox thermal-1d-dirichlet-constant-k.fee 
0.5
$ 
```

The mesh is assumed to have been already created with [Gmsh](http://gmsh.info/) (or any other pre-processing tool and converted to `.msh` format with [Meshio](https://github.com/nschloe/meshio) for example). This assumption follows the _rule of composition_  and prevents the actual input file to be polluted with mesh-dependent data (such as node coordinates and/or nodal loads) so as to keep it simple and make it [Git](https://git-scm.com/)-friendly (_rule of generation_). The only link between the mesh and the FeenoX input file is through physical groups (in the case above `left` and `right`) used to set boundary conditions and/or material properties.

Another design-basis decision is that **similar problems ought to have similar inputs** (_rule of least surprise_). So in order to have a space-dependent conductivity, we only have to replace one line in the input above: instead of defining a scalar\ $k$ we define a function of\ $x$ (we also update the output to show the analytical solution as well):

```{.feenox include="thermal-1d-dirichlet-space-k.fee"}
```

```terminal
$ feenox thermal-1d-dirichlet-space-k.fee 
0.584959	0.584963
$
```

FeenoX has an **everything is an expression** design principle, meaning that any numerical input can be an algebraic expression (e.g. `T(1/2)` is the same as `T(0.5)`). If we want to have a temperature-dependent conductivity (which renders the problem non-linear) we can take advantage of the fact that $T(x)$ is available not only as an argument to `PRINT` but also for the definition of algebraic functions:

```{.feenox include="thermal-1d-dirichlet-temperature-k.fee"}
```

```terminal
$ feenox thermal-1d-dirichlet-temperature-k.fee 
0.581139	0.581139
$
```
