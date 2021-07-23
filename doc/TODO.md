 * FeenoX and the UNIX philosophy
 
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
 
 * IAEA 2D/3D: eighth symmetry, circular vessel, unstructured triangles in the moderator, structured quadrangles in the in the fuel
 
 * documentation analogy: ccx is like word, most people use it but the results are ugly and it is inflexible
   libraries are like latex (or even raw tex), very powerful but quite cumbersome
   CAEplex is like google docs
   FeenoX is like markdown, quick and easy but eventually flexible and powerful when combined with pandoc. Maybe it has a stiff learning curve but it is very natural once you think it through
 
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
   
