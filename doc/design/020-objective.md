## Objective {#sec:objective}

The main objective of the tool is to be able to solve engineering problems which are usually casted as differential-algebraic equations (DAEs) or partial differential equations (PDEs), such as

 * dynamical systems
 * plant control dynamics
 * mechanical elasticity
 * heat conduction
 * structural modal analysis
 * electromagnetism
 * chemical diffusion
 * computational fluid dynamics
 * ...

on one or more manistream cloud servers, i.e. computers with an architecture (i.e. hardware and operating systems, futher discussed in [@sec:architecture]) that allows them to be available online and accessed remotely either interactively or automatically by other computers as well. Other architectures such as high-end desktop personal computers or even low-end laptops might be supported but they should not the main target. 
 
The initial version of the tool must be able to handle a subset of the above list of problem types.
Afterward, the set of supported problem types, models, equations and features of the tool should grow to include other models as well, as required in\ [@sec:extensibility].
