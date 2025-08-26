
The main objective of the tool is to be able to solve engineering problems which are usually casted as differential-algebraic equations (DAEs) or partial differential equations (PDEs), such as

 * heat conduction
 * mechanical elasticity
 * structural modal analysis
 * mechanical frequency studies
 * electromagnetism
 * chemical diffusion
 * process control dynamics
 * computational fluid dynamics
 * ...

on one or more mainstream cloud servers, i.e. computers with hardware and operating systems (further discussed in @sec:architecture) that allows them to be available online and accessed remotely either interactively or automatically by other computers as well. Other architectures such as high-end desktop personal computers or even low-end laptops might be supported but they should not the main target (i.e. the tool has to be cloud-first but laptop-friendly).

The initial version of the tool must be able to handle a subset of the above list of problem types.
Afterward, the set of supported problem types, models, equations and features of the tool should grow to include other models as well, as required in @sec:extensibility.
