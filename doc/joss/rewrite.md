- Section comparing how to solve a PDE with other guys (only FOSS), and answer "solving PDEs by writing weak forms in FEniCS and FreeFEM is totally different and much easier than developing the stiffness matrices from scratch as needed in this library".
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
      
- [ ] and current limitations
- [ ] and expected behavior when running examples (including ways to improve the solver).

- [ ] Contributing
      - How to create a PR
      - Reviews
      - Tests
      
- [ ] Fun & games
      - Maze
      - Lorenz

- [ ] What people say
      - igor2
      - vitor (about milonga)
      - allvar: makes my life easy
      
- [ ] optimization
      - neural concept?
      - uno que pidió una API por mail
      - ver runs, repos @ projects.seamplex.com
      
- [ ] scalability
      - vs. who?

- [ ] Adding a new PDE to `src/pdes` in doc

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
