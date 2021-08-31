FeenoX is a computational tool that can solve engineering problems which are usually casted as differential-algebraic equations (DAEs) or partial differential equations (PDEs). In particular, it can solve

 * dynamical systems defined by a set of user-provided DAEs (such as plant control dynamics for example)
 * mechanical elasticity
 * heat conduction
 * structural modal analysis
 * neutron diffusion
 * neutron transport

FeenoX reads a plain-text input file which contains the problem definition and writes 100%-user defined results in ASCII (through *PRINT* or other user-defined output instructions within the input file). For PDE problems, it needs a reference to at least one **gmsh**`(1)` mesh file for the discretization of the domain. It can write post-processing views in either _.msh_ or _.vtk_ formats. 

Keep in mind that FeenoX is just a back end reading a set of input files and writing a set of output files following the design philosophy of UNIX (separation, composition, representation, economy, extensibility, etc). Think of it as a transfer function between input files and output files:

```{.include}
transfer.md
```

Following the UNIX programming philosophy, there are no graphical interfaces attached to the FeenoX core, although a wide variety of pre and post-processors can be used with FeenoX. See for example <https://www.caeplex.com> for a web-based interface.
