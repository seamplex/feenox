      
- [ ] Illustrate
      - Extents
      - Capabilities
      - Usefulness
      
- [ ] FeenoX is _program_, **not a library**! It is not something you link against. It is something you run and feed the input file at runtime.

- [ ] List of youtube videos
      - In doc/README

- [ ] Explain why it is different
      - Unix philosophy
      - Cloud first: This particular aspect has no direct implication of being "cloud"-friendly. Almost every good library or application that can be deployed on Docker supports good CI, deployment, status reporting, etc. I suggest focusing on CAEPlex to emphasize cloud computing.
      - FOSS
      - Input syntax
         - simple problems simple inputs
         - self descriptive
         - one to one correspondence
         - etc.
         
- [ ] Explain idea of C entry points
      - Devs $\neq$ users
      - Development is not straightforward but _possible_

      
- [ ] Section comparing how to solve a PDE with other guys (only FOSS), and answer "solving PDEs by writing weak forms in FEniCS and FreeFEM is totally different and much easier than developing the stiffness matrices from scratch as needed in this library".
      - Section "how to solve..."
      - doc/differences
      - LE10? Some IAEA case?
      - Sparselizard
      - Code Aster
      - CalculiX
      - FEniCS
      - FreeFEM
      - MoFEM
      - ...
      
- [ ] "While the library contains solver capabilities only for a few basic problems (mostly steady-state/static, transient heat transfer but mostly linear problems) and offers FEM ingredients for further developments, the development of solvers for other PDEs, particularly for nonlinear problems, is not as simple as indicated in the paper."
      - Neutronics
      - DAEs
      
- [ ] State that "development of solvers for other PDEs, particularly nonlinear problems, is not simple" but **possible**

- [ ] Show
      - Flexibility
         - Parametric sweeps
         - Optimization
         - Material properties
         - Boundary conditions
      - Scalability
      - Performance
      
- [ ] FeenoX lessens the burden of PDE solving, not the burden of solver development!

- [ ] "If documented better, it may also be a simple tool to use for students before writing their own computational solver implementations."

- [ ] "I am also confused by the terminology "cloud-first". I assumed this meant that it could be deployed on cloud computing platforms like AWS. However, this is unclear and misleading."

- [ ] deficiency in the documentation to clarify better the classes of problems supported,
- [ ] usage options
- [ ] and current limitations
- [ ] and expected behavior when running examples (including ways to improve the solver).

- [ ] Contributing
      - How to create a PR
      - Reviews
      - Tests
      
- [ ] Highlight verification in academics
      - MMS
      - test suite: unit, regression and (primitive) verification
      
- [ ] Fun & games
      - Maze
      - Lorenz


- [ ] Remark SRS + SDS
      - hackers
      - academics

- [ ] cloud includes
      - thin clients
      - scripted studies
      
- [ ] parametric
      - link to SDS
      
- [ ] why is it different?
      - no other guy fulfills the SRS
      
- [ ] What people say
      - igor2
      - vitor (about milonga)
      - allvar: makes my life easy
      
- [ ] NOT A LIBRARY!
      - unix
      
- [ ] optimization
      - neural concept?
      - uno que pidió una API por mail
      - ver runs, repos @ projects.seamplex.com
      
- [ ] scalability
      - vs. who?

- [ ] mention PETSc & SLEPc in unix & academics
      - glue layer
      - for linear elasticity
      
      ```
      
      Gmsh   -> mesh                                                                             console
                        --->  Feenox -> K & b -> PETSc Ku = b -> Feenox -> stresses from u --->  
      editor -> input                                                                            .vtk -> paraview
      
## comparisons

### sparselizard

 - no odes/math
 - have to compile each problem
 - no MPI (or yes?)
 
### calculix

 - inintelligible fortran
 - no odes/math
 - no multi-node MPI
 - mesh in inp (nodal forces)
 - no parametric
 - cluttered output
 - non-trivial input
 
### aster

 - no odes/math
 - cluttered output
 - difficult to compile/install
 - non-trivial input
 
### fenics

 ?
 
### freefem

 ?
 
### elmer

 ? 
