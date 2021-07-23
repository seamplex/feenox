For example, we can solve the [NAFEMS\ LE11](https://www.nafems.org/publications/resource_center/p18/) “Solid cylinder/Taper/Sphere-Temperature” benchmark like

```{.feenox style=feenox}
READ_MESH nafems-le11.msh DIMENSIONS 3
PROBLEM mechanical

# linear temperature gradient in the radial and axial direction
T(x,y,z) = sqrt(x^2 + y^2) + z

# Boundary conditions
BC xz     symmetry  # same as v=0 but "symmetry" follows the statement
BC yz     symmetry  # ide with u=0
BC xy     w=0
BC HIH'I' w=0

# material properties (isotropic & uniform so we can use scalar constants)
E = 210e3*1e6       # mesh is in meters, so E=210e3 MPa -> Pa
nu = 0.3            # dimensionless
alpha = 2.3e-4      # in 1/ºC as in the problem

SOLVE_PROBLEM
WRITE_MESH nafems-le11.vtk VECTOR u v w   T sigma1 sigma2 sigma3 sigma sigmaz
PRINT "sigma_z(A) = " sigmaz(0,1,0)/1e6 "MPa"
```

Another example would be the famous chaotic [Lorenz’ dynamical system](http://en.wikipedia.org/wiki/Lorenz_system)---the one of the butterfly---whose differential equations are

```{=plain}
dx/dt = σ (y-x)  
dy/dt = x (r-z) - y
dz/dt = x y - b z
```

```{=latex}
\begin{equation*}
\begin{cases}
\dot{x} &= \sigma \cdot (y - x)  \\
\dot{y} &= x \cdot (r - z) - y   \\
\dot{z} &= x y - b z       \\
\end{cases}
\end{equation*}
```

::: {.not-in-format .plain .latex }
$$\dot{x} = \sigma \cdot (y - x)$$  
$$\dot{y} = x \cdot (r - z) - y$$  
$$\dot{z} = x \cdot y - b \cdot z$$  
:::

where $\sigma=10$, $b=8/3$ and $r=28$ are the classical parameters that generate the butterfly as presented by Edward Lorenz back in his seminal 1963 paper [Deterministic non-periodic flow](http://journals.ametsoc.org/doi/abs/10.1175/1520-0469%281963%29020%3C0130%3ADNF%3E2.0.CO%3B2). We can solve it with FeenoX by writing the equations in the input file as naturally as possible, as illustrated in the input file that follows:

```{.feenox style=feenox}
PHASE_SPACE x y z     # Lorenz attractor’s phase space is x-y-z
end_time = 40         # we go from t=0 to 40 non-dimensional units

sigma = 10            # the original parameters from the 1963 paper
r = 28
b = 8/3

x_0 = -11             # initial conditions
y_0 = -16
z_0 = 22.5

# the dynamical system's equations written as naturally as possible
x_dot = sigma*(y - x)
y_dot = x*(r - z) - y
z_dot = x*y - b*z

PRINT t x y z        # four-column plain-ASCII output
```


Please note the following two points about both cases above:

 1. The input files are very similar to the statements of each problem in plain English words (_rule of clarity_). Take some time to read the [problem statement of the NAFEMS\ LE11 benchmark](doc/design/nafems-le11/nafems-le11.png) and the FeenoX input to see how well the latter matches the former. Same for the Lorenz’ chaotic system. Those with some experience may want to compare them to the inputs decks (sic) needed for other common FEA programs.
 2. By design, 100% of FeenoX’ output is controlled by the user. Had there not been any `PRINT` or `WRITE_MESH` instructions, the output would have been empty, following the _rule of silence_. This is a significant change with respect to traditional engineering codes that date back from times when one CPU hour was worth dozens (or even hundreds) of engineering hours. At that time, cognizant engineers had to dig into thousands of lines of data to search for a single individual result. Nowadays, following the _rule of economy_, it is actually far easier to ask the code to write only what is needed in the particular format that suits the user.
