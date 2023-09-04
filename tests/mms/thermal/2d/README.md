# MMS verification: thermal 2D square

This directory shows how to perform verification of the 2D thermal solver using the Method of Manufactured Solutions.

## Dependencies

 * [FeenoX](https://www.seamplex.com/feenox)
 * [Gmsh](http://gmsh.info/)
 * [Maxima](https://maxima.sourceforge.io/)
 
## Execution

Run the script `run.sh` (and `clean.sh` if you want to re-run):

```
./clean.sh
./run.sh
```

You'll see 8 columns:

 1. logarithm of the mesh size $\log(h)$
 2. logarithm of the $L-2$ error $\log(e_2)$
 3. logarithm of the $L-\infty$ error $\log(e_\infty)$
 4. $c$ factor for mesh size $h \propto 1/c$
 5. number of cells (volumetric elements)
 6. number of nodes (degrees of freedom)
 7. total memory in Mb
 8. wall time in seconds
 
### Changing parameteres
 
The assumed temperature solution $T(x,y)$ and the conductivity $k(x,y)$ are read from the main FeenoX input `thermal-square.fee`, like

```feenox
# MMS data, set T_mms(x) and k_mms(x) as desired
T_mms(x,y) = 1 + sin(2*x)^2 * cos(3*y)^2
k_mms(x,y) = 1 + x - 0.5*y
```

Feel free to modify the expressions to something you like.


Maxima is then called with both $T(x,y)$ and $k(x,y)$ to compute the source term $q(x,y)$ and the partial derivatives of the temperature to compute the heat fluxes in both directions for the Neumann BCs.

The `run.sh` script sweeps over (you can edit the script to change what it sweeps over):

 * BCs
   - dirichlet
   - neumann
 * element types
   - tri3
   - tri6
   - quad4
   - quad8
   - quad9
 * meshing algorithms
   - struct
   - frontal
   - dealaunay
 * refinement factors $h=1/c$, $c=$ 8 16 24 28 32 36 40 48 64 72 96 104 112




Each combination of BC, element type and meshing algorithm has a data file with the errors ($e_2$ and $e_\infty$) vs. the element size\ $h$, which is fitted with FeenoX and plotted with Pyxplot:

```
pyxplot thermal-square.ppl
```

There's also a markdown file with a table listing the order of convergence of each combination.

# Detailed explanation

To be done.



# Results


