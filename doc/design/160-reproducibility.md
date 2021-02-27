## Reproducibility and traceability 

The full source code and the documentation of the tool ought to be maintained under a control version system hosted on a public server accessible worldwide without needing any special credentials to get a copy of the code. 

All the information needed to solve a particular problem (i.e. meshes, boundary conditions, spatially-distributed material properties, etc.) should be  generated from a very simple set of files which ought to be susceptible of being tracked with current state-of-the-art version control systems.

simple <-> simple

similar <-> similar

Mesh data should be mixed with the problem data like material properties or boundary conditions.
Changes in the mesh should be tracked on the files needed to create the mesh and not on the mesh itself.
