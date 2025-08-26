 * Documentation should be like code: if paragraph appear more than once, then a separate file should be created and included in the needed locations.
 
 * FeenoX focuses on:
 
    a. cloud-firstness
    b. parametric runs: script-friendly through command-line arguments and ASCII files
    c. material properties: everything is an expression
    d. LLM-friendlyness

 * NAFEMS LE10 & LE11: keep a single case in `differences` and create a new one in `cases` with a full parametric run

   ```
   READ_MESH le10-$1.msh
   ...
   PRINT "sigma_y @ D = " ... "for $1"
   ```
   
   Do a parametric run with a shell script: tet-unstructured-c, hex-structured-c
   
   ```
   ndofs sigma_y strain energy wall memory 
   ```
   
   Use GAMG & MUMPS
   
   Compare both cases (or one and one) to CalculiX & CodeAster. Link to CoFEA.
   
   
 * FeenoX and the Unix philosophy
 
 * FeenoX & CalculiX: blog post? (or Why I don't like CalculIX)
 
   1. start with a ccx tutorial - point & click
   2. do the same with CAEplex
   3. do the same with FeenoX
   4. do it right, parametric
   
   ccx is a copy, not any real improvement
   most people use it as gratis not as free---which is fine, even great!
 
 * how to solve a 30-year-old problem? hint: read the problem from the back, first what it asks for and then what information it gives in order to solve it---same as IAEA
 
   1. CAD from src
   2. locally-refined mesh
   3. mesh partition (TBD)
   4. get $\sigma_y$ and nothing else (at least not to the screen)
   5. parametrically refine & check convergence
 
 * "use first-order elements because it takes too long with second-order elements"
 
 * input files: *.fee
   scripts: shell, python
   extras: CAD, gmsh, Gnuplot/Pyxplot/Matplotlib, paraview
   
   RUN Gmsh, FeenoX, Post
   
   plots: pdf, svg
   3d snapshots: png
   post-processing interactive views: Gmsh, Paraview, ccg, PrePoMax, webgl
   
 * V&V
 
   * MMS
   * NAFEMS
      - LE1
      - LE10
      - FV52 unconstrained modal
      - T1 linear elasticity with thermal expansion
      - T2 radiation with analytical solution
      - T3 1D transient thermal
      - T4 2D thermal with convection
      - challenge quad4
   * Parallellpied whose E depends no temperature
   
 * aalib for man

# Lua filters

```
sed -i 's/@verbatim/@smallformat\n@verbatim/' feenox-desc.texi
sed -i 's/@end verbatim/@end verbatim\n@end smallformat/' feenox-desc.texi         
```
   
```
for i in logo nafems-le10-problem-input lorenz; do
  if [ ! -e $i.pdf ]; then
    inkscape --export-type=pdf ${i}.svg
  fi
done
```
