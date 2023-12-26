
- [ ] Separate README in three:
      - Academics: those who know the math behind PDEs and the FEA method
      - Hackers: those who are comfortable with Unix terminals and cloud servers
      - Engineers: those who need to solve industrial problems
      
- [ ] Illustrate
      - Extents
      - Capabilities
      - Usefulness
      
- [ ] FeenoX is _program_, **not a library**! It is not something you link against. It is something you run and feed the input file at runtime.

- [ ] List of youtube videos

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

# Academics

## What

A program, i.e. something your run, reads the problem in a plain text file containing definitions and instructions, solves it, writes the output (if asked for with instructions in the input file) and returns back to the shell.
It is deliberately not a library, i.e. something you have to link your code against. Link to SRS.


## How

By leveraging the Unix programming philosophy to come up with a cloud-first tool suitable to serve as the back end of web-based interfaces such as CAEplex.
A mathematical framework with C entry points that tell it what to put in the global stiffness matrix, residuals, etc.


## Why

Because there are either

 a. libraries which need code to use them, 
 b. end-user programs which need a GUI

FeenoX sits in the middle:

 * to solve a problem one needs to prepare an input file (not a script) which is read at run-time, not code which calls a library
 * these input files can expand generic command-line options using Bash syntax as `$1`, `$2`, etc, which allows for parametric or optimization loops driven by higher-level scripts
 * for solving PDEs, the input file has to refer to at least one Gmsh `.msh` file that defines the domain where the PDE is solved
 * the material properties and boundary condition are defined using physical groups and not individual nodes nor elements, so the input file is independent of the mesh and thus can be tracked with Git to increase traceability and repeatability
 * it uses the Unix philosophy which, among others, separates policy from mechanism and thus FeenoX is a natural choice for web-based interfaces like CAEplex
 

# Hackers

## Why

Word - Markdown - TeX

## How

A cloud-first computational engineering tool based on the Unix philosophy.


## What

SRS + SDS


# Engineers


## How

How it works: terminal & cloud.
A transfer function

## What

Tutorials

## Why

Why it works the way it does?
Because it is cloud first and its objective is to power web-based interfaces like CAEplex.
Depending on the complexity of the case, CAEplex might be enough or not. In the latter, one has to see what's sitting under the hood.
