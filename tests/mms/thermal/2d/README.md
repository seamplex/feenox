# MMS verification: thermal 2D square

The script `thermal-square.sh` sweeps over

 * BCs: dirichlet, neumann
 * element types: tri3, tri6, quad4, quad8, quad9
 * meshing algorithms: struct, frontal, dealaunay
 * refinement factors $h=1/c$, $c=$ 8 16 24 28 32 36 40 48 64 72 96 104 112
 
The assumed temperature solution $T(x,y)$ is read from the main FeenoX input `thermal-square.fee` with

```
$ grep "T_mms(x,y) :=" thermal-square.fee | sed 's/T_mms(x,y)//' | sed 's/:=//'
  1 + sin(2*x)^2 * cos(3*y)^2
$ 
```

Maxima is then called with both $T(x,y)$ and $k(x,y)$ to compute the source term $q(x,y)$ and the partial derivatives of the temperature to compute the heat fluxes in both directions for the Neumann BCs.

Each combination of BC, element type and meshing algorithm has a data file with the errors ($e_2$ and $e_\infty$) vs. the element size\ $h$, which is fitted with FeenoX and plotted with Pyxplot. 

