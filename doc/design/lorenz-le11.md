For example, the famous chaotic [Lorenz’ dynamical system](http://en.wikipedia.org/wiki/Lorenz_system)---the one of the butterfly---whose differential equations are

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

where $\sigma=10$, $b=8/3$ and $r=28$ are the classical parameters that generate the butterfly as presented by Edward Lorenz back in his seminal 1963 paper [Deterministic non-periodic flow](http://journals.ametsoc.org/doi/abs/10.1175/1520-0469%281963%29020%3C0130%3ADNF%3E2.0.CO%3B2), can be solved with FeenoX by writing the equations in the input file as naturally as possible, as illustrated in the input file that follows:

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
x_dot .= sigma*(y - x)
y_dot .= x*(r - z) - y
z_dot .= x*y - b*z

PRINT t x y z        # four-column plain-ASCII output
```

Another example would be to solve the [NAFEMS\ LE11](https://www.nafems.org/publications/resource_center/p18/) “Solid cylinder/Taper/Sphere-Temperature” benchmark like

```{.feenox style=feenox}
READ_MESH nafems-le11.msh DIMENSIONS 3
PROBLEM mechanical

# linear temperature gradient in the radial and axial direction
T(x,y,z) := sqrt(x^2 + y^2) + z

# Boundary conditions
BC xz     v=0
BC yz     u=0
BC xy     w=0
BC HIH'I' w=0

# material properties (isotropic & uniform so we can use scalar constants)
E = 210e3*1e6       # mesh is in meters, so E=210e3 MPa -> Pa
nu = 0.3
alpha = 2.3e-4      # in 1/ºC as in the problem

SOLVE_PROBLEM
WRITE_MESH nafems-le11.vtk VECTOR u v w   T sigma1 sigma2 sigma3 sigma sigmaz
PRINT "sigma_z(A) = " sigmaz(0,1,0)/1e6 "MPa"
```

Please note the following two points about both cases above:

 1. the input files are very similar to the statements of each problem in plain English words as in the [UNIX rule of clarity](http://catb.org/~esr/writings/taoup/html/ch01s06.html).
 2. 100% of FeenoX’ output is controlled by the user. Had there not been any `PRINT` or `WRITE_MESH` instructions, the output would have been empty, following the [UNIX rule of silence](http://catb.org/~esr/writings/taoup/html/ch01s06.html).
