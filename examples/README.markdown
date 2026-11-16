# FeenoX annotated examples

- [<span class="toc-section-number">1</span> Examples by problem type]
- [<span class="toc-section-number">2</span> Test suite]
- [<span class="toc-section-number">3</span> Detailed list of examples]

  [<span class="toc-section-number">1</span> Examples by problem type]: #examples-by-problem-type
  [<span class="toc-section-number">2</span> Test suite]: #test-suite
  [<span class="toc-section-number">3</span> Detailed list of examples]:
    #detailed-list-of-examples

# Examples by problem type

The FeenoX examples are divided by the type of problem they solve:

1.  [Basic mathematics]
2.  [Systems of ODEs/DAEs]
3.  [Laplace’s equation]
4.  [Heat conduction]
5.  [Elasticity (small and large def.)]
6.  [Modal analysis]
7.  [Neutron diffusion]
8.  [Neutron $`S_N`$]
9.  [Thermal hydraulics]

> Each type of partial differential equation (i.e. from Laplace
> downward) is implemented in a subdirectory within [`src/pde`] of the
> source tree. Feel free to…
>
> - ask questions in the [Github discussion page]
> - propose features, corrections, improvements, etc.
> - create a pull request for other PDEs. Candidates would be
>   - fluid mechanics
>   - thermal hydraulics
>   - electromagnetism
>   - non-linear elasticity
>   - cell-level neutronics (i.e. method of characteristics, collision
>     probabilities, etc)

All the files needed to run these examples are available in the
[examples] directory of the [Git repository] and any of the [tarballs],
both [source] and [binary].

For a presentation of these examples, see the “FeenoX Overview
Presentation” (August 2021).

- [Recording (audio in Spanish, slides in English)]
- [Slides in PDF]
- [Markdown examples sources]

  [Basic mathematics]: basic.md
  [Systems of ODEDAEs]: daes.markdown
  [Laplace’s equation]: laplace.md
  [Heat conduction]: thermal.md
  [Elasticity (small and large def.)]: mechanical.md
  [Modal analysis]: modal.md
  [Neutron diffusion]: neutron_diffusion.md
  [Neutron $`S_N`$]: neutron_sn.md
  [Thermal hydraulics]: thermal_hydraulics.md
  [`src/pde`]: https://github.com/seamplex/feenox/tree/main/src/pdes
  [Github discussion page]: https://github.com/seamplex/feenox/discussions
  [examples]: https://github.com/seamplex/feenox/tree/main/examples
  [Git repository]: https://github.com/seamplex/feenox
  [tarballs]: https://www.seamplex.com/feenox/download.html
  [source]: https://www.seamplex.com/feenox/dist/src
  [binary]: https://www.seamplex.com/feenox/dist/linux
  [Recording (audio in Spanish, slides in English)]: https://youtu.be/-RJ5qn7E9uE
  [Slides in PDF]: https://www.seamplex.com/feenox/doc/2021-feenox.pdf
  [Markdown examples sources]: https://github.com/gtheler/2021-presentation

# Test suite

The `tests` [directory in the Github repository] has dozens of test
cases which can be used as examples for reference and for mathematical
verification of the results obtained with FeenoX. These are run when
doing `make check` after [compiling the source code].

The test cases usually return a single number (which should be near
zero) comparing the numerical result with the expected one. Feel free to
propose benchmark problems to add to the suite.

  [directory in the Github repository]: https://github.com/seamplex/feenox/tree/main/tests
  [compiling the source code]: .doc/compile.markdown

# Detailed list of examples

- Basic mathematics
  - [Hello World (and Universe)!]
  - [Ten ways of computing *π*]
  - [Interpolating a two-dimensional function]
  - [Equality in the Cauchy-Schwarz inequality]
  - [Financial decisions under inflation]
  - [Discrete compounding and continuous growth]
  - [The logistic map]
  - [The Fibonacci sequence]
    - [Using the closed-form formula as a function]
    - [Using a vector]
    - [Solving an iterative problem]
  - [Computing the derivative of a function as a Unix filter]
  - [Length of a quadratic arc]
  - [A stochastic urn model]
  - [On the evaluation of thermal expansion coefficients]
  - [Buffon’s needle]
- Ordinary Differential Equations & Differential-Algebraic Equations
  - [Lorenz’ attractor—the one with the butterfly]
  - [The double pendulum]
  - [Vertical boiling channel]
    - [Original Clausse-Lahey formulation with uniform power
      distribution]
    - [Arbitrary power distribution]
  - [Reactor point kinetics]
    - [Cinética puntual directa con reactividad vs. tiempo]
    - [Cinética inversa]
    - [Control de inestabilidades de xenón]
    - [Mapas de diseño]
  - [Effect of top spin in a tennis ball]
- Laplace’s equation
  - [How to solve a maze without AI]
    - [Transient top-down]
    - [Transient bottom-up]
  - [Potential flow around an airfoil profile]
- Heat conduction
  - [Thermal slabs]
    - [One-dimensional linear]
  - [Transient heat conduction from steady-state by “turning off” BCs]
  - [Non-dimensional transient heat conduction on a cylinder]
  - [Non-dimensional transient heat conduction with time-dependent
    properties]
- Elasticity (small and large deformation)
  - [NAFEMS LE10 “Thick plate pressure” benchmark]
  - [NAFEMS LE11 “Solid Cylinder/Taper/Sphere-Temperature” benchmark]
  - [NAFEMS LE1 “Elliptical membrane” plane-stress benchmark]
  - [Parametric study on a cantilevered beam]
  - [Parallelepiped whose Young’s modulus is a function of the
    temperature]
    - [Thermal problem]
    - [Mechanical problem]
  - [Orthotropic free expansion of a cube]
  - [Strain energy of a cube under force and displacement control]
    - [Axial force]
    - [Transverse force]
    - [Prescribed axial displacement]
  - [Thermo-elastic expansion of finite cylinders]
  - [Temperature-dependent material properties]
  - [Two cubes compressing each other]
  - [Steel/aluminum paradox]
  - [NAFEMS GNL5 “Large-deformation beam”]
- Mechanical modal analysis
  - [Optimizing the length of a tuning fork]
  - [Bunny modes with three support conditions]
    - [Fixed base]
    - [Free-free vibration]
    - [Radial and tangential base constraints]
  - [Five natural modes of a cantilevered wire]
- Neutron diffusion
  - [IAEA 2D PWR Benchmark]
  - [IAEA 3D PWR Benchmark]
  - [Cube-spherical bare reactor]
  - [Illustration of the XS dilution & smearing effect]
- Neutron transport using $`S_N`$
  - [Reed’s problem]
  - [Azmy’s problem]
    - [Second-order complete structured rectangular grid]
    - [Fist-order locally-refined unstructured triangular grid]
    - [Flux profiles with ray effect]
- Thermal hydraulics
  - [RELAP7 Example 1: A simple pipe flow problem]
  - [RELAP7 Example 2: A transient pipe flow problem]
  - [RELAP7 Example 3: Adding heat to a simple pipe flow problem]
  - [RELAP7 Example 5: A core channel with a fuel rod]
  - [RELAP7 Example 6: A two-pipe flow problem]
  - [RELAP7 Example 7: A junction with multiple pipes in and out]
  - [RELAP7 Example 8: A simple pipe loop with a pump]
  - [RELAP7 Example 9: A heat exchanger]
  - [RELAP7 Example 10: A loop with a core channel and a heat exchanger]

  [Hello World (and Universe)!]: https://seamplex.com/feenox/examples/basic.html#hello-world-and-universe
  [Ten ways of computing *π*]: https://seamplex.com/feenox/examples/basic.html#ten-ways-of-computing-pi
  [Interpolating a two-dimensional function]: https://seamplex.com/feenox/examples/basic.html#interpolating-a-two-dimensional-function
  [Equality in the Cauchy-Schwarz inequality]: https://seamplex.com/feenox/examples/basic.html#equality-in-the-cauchy-schwarz-inequality
  [Financial decisions under inflation]: https://seamplex.com/feenox/examples/basic.html#financial-decisions-under-inflation
  [Discrete compounding and continuous growth]: https://seamplex.com/feenox/examples/basic.html#discrete-compounding-and-continuous-growth
  [The logistic map]: https://seamplex.com/feenox/examples/basic.html#the-logistic-map
  [The Fibonacci sequence]: https://seamplex.com/feenox/examples/basic.html#the-fibonacci-sequence
  [Using the closed-form formula as a function]: https://seamplex.com/feenox/examples/basic.html#using-the-closed-form-formula-as-a-function
  [Using a vector]: https://seamplex.com/feenox/examples/basic.html#using-a-vector
  [Solving an iterative problem]: https://seamplex.com/feenox/examples/basic.html#solving-an-iterative-problem
  [Computing the derivative of a function as a Unix filter]: https://seamplex.com/feenox/examples/basic.html#computing-the-derivative-of-a-function-as-a-unix-filter
  [Length of a quadratic arc]: https://seamplex.com/feenox/examples/basic.html#length-of-a-quadratic-arc
  [A stochastic urn model]: https://seamplex.com/feenox/examples/basic.html#a-stochastic-urn-model
  [On the evaluation of thermal expansion coefficients]: https://seamplex.com/feenox/examples/basic.html#on-the-evaluation-of-thermal-expansion-coefficients
  [Buffon’s needle]: https://seamplex.com/feenox/examples/basic.html#buffons-needle
  [Lorenz’ attractor—the one with the butterfly]: https://seamplex.com/feenox/examples/daes.html#lorenz-attractorthe-one-with-the-butterfly
  [The double pendulum]: https://seamplex.com/feenox/examples/daes.html#the-double-pendulum
  [Vertical boiling channel]: https://seamplex.com/feenox/examples/daes.html#vertical-boiling-channel
  [Original Clausse-Lahey formulation with uniform power distribution]: https://seamplex.com/feenox/examples/daes.html#original-clausse-lahey-formulation-with-uniform-power-distribution
  [Arbitrary power distribution]: https://seamplex.com/feenox/examples/daes.html#arbitrary-power-distribution
  [Reactor point kinetics]: https://seamplex.com/feenox/examples/daes.html#reactor-point-kinetics
  [Cinética puntual directa con reactividad vs. tiempo]: https://seamplex.com/feenox/examples/daes.html#cinética-puntual-directa-con-reactividad-vs.-tiempo
  [Cinética inversa]: https://seamplex.com/feenox/examples/daes.html#cinética-inversa
  [Control de inestabilidades de xenón]: https://seamplex.com/feenox/examples/daes.html#control-de-inestabilidades-de-xenón
  [Mapas de diseño]: https://seamplex.com/feenox/examples/daes.html#mapas-de-diseño
  [Effect of top spin in a tennis ball]: https://seamplex.com/feenox/examples/daes.html#effect-of-top-spin-in-a-tennis-ball
  [How to solve a maze without AI]: https://seamplex.com/feenox/examples/laplace.html#how-to-solve-a-maze-without-ai
  [Transient top-down]: https://seamplex.com/feenox/examples/laplace.html#transient-top-down
  [Transient bottom-up]: https://seamplex.com/feenox/examples/laplace.html#transient-bottom-up
  [Potential flow around an airfoil profile]: https://seamplex.com/feenox/examples/laplace.html#potential-flow-around-an-airfoil-profile
  [Thermal slabs]: https://seamplex.com/feenox/examples/thermal.html#thermal-slabs
  [One-dimensional linear]: https://seamplex.com/feenox/examples/thermal.html#one-dimensional-linear
  [Transient heat conduction from steady-state by “turning off” BCs]: https://seamplex.com/feenox/examples/thermal.html#transient-heat-conduction-from-steady-state-by-turning-off-bcs
  [Non-dimensional transient heat conduction on a cylinder]: https://seamplex.com/feenox/examples/thermal.html#non-dimensional-transient-heat-conduction-on-a-cylinder
  [Non-dimensional transient heat conduction with time-dependent properties]:
    https://seamplex.com/feenox/examples/thermal.html#non-dimensional-transient-heat-conduction-with-time-dependent-properties
  [NAFEMS LE10 “Thick plate pressure” benchmark]: https://seamplex.com/feenox/examples/mechanical.html#nafems-le10-thick-plate-pressure-benchmark
  [NAFEMS LE11 “Solid Cylinder/Taper/Sphere-Temperature” benchmark]: https://seamplex.com/feenox/examples/mechanical.html#nafems-le11-solid-cylindertapersphere-temperature-benchmark
  [NAFEMS LE1 “Elliptical membrane” plane-stress benchmark]: https://seamplex.com/feenox/examples/mechanical.html#nafems-le1-elliptical-membrane-plane-stress-benchmark
  [Parametric study on a cantilevered beam]: https://seamplex.com/feenox/examples/mechanical.html#parametric-study-on-a-cantilevered-beam
  [Parallelepiped whose Young’s modulus is a function of the temperature]:
    https://seamplex.com/feenox/examples/mechanical.html#parallelepiped-whose-youngs-modulus-is-a-function-of-the-temperature
  [Thermal problem]: https://seamplex.com/feenox/examples/mechanical.html#thermal-problem
  [Mechanical problem]: https://seamplex.com/feenox/examples/mechanical.html#mechanical-problem
  [Orthotropic free expansion of a cube]: https://seamplex.com/feenox/examples/mechanical.html#orthotropic-free-expansion-of-a-cube
  [Strain energy of a cube under force and displacement control]: https://seamplex.com/feenox/examples/mechanical.html#strain-energy-of-a-cube-under-force-and-displacement-control
  [Axial force]: https://seamplex.com/feenox/examples/mechanical.html#axial-force
  [Transverse force]: https://seamplex.com/feenox/examples/mechanical.html#transverse-force
  [Prescribed axial displacement]: https://seamplex.com/feenox/examples/mechanical.html#prescribed-axial-displacement
  [Thermo-elastic expansion of finite cylinders]: https://seamplex.com/feenox/examples/mechanical.html#thermo-elastic-expansion-of-finite-cylinders
  [Temperature-dependent material properties]: https://seamplex.com/feenox/examples/mechanical.html#temperature-dependent-material-properties
  [Two cubes compressing each other]: https://seamplex.com/feenox/examples/mechanical.html#two-cubes-compressing-each-other
  [Steel/aluminum paradox]: https://seamplex.com/feenox/examples/mechanical.html#steelaluminum-paradox
  [NAFEMS GNL5 “Large-deformation beam”]: https://seamplex.com/feenox/examples/mechanical.html#nafems-gnl5-large-deformation-beam
  [Optimizing the length of a tuning fork]: https://seamplex.com/feenox/examples/modal.html#optimizing-the-length-of-a-tuning-fork
  [Bunny modes with three support conditions]: https://seamplex.com/feenox/examples/modal.html#bunny-modes-with-three-support-conditions
  [Fixed base]: https://seamplex.com/feenox/examples/modal.html#fixed-base
  [Free-free vibration]: https://seamplex.com/feenox/examples/modal.html#free-free-vibration
  [Radial and tangential base constraints]: https://seamplex.com/feenox/examples/modal.html#radial-and-tangential-base-constraints
  [Five natural modes of a cantilevered wire]: https://seamplex.com/feenox/examples/modal.html#five-natural-modes-of-a-cantilevered-wire
  [IAEA 2D PWR Benchmark]: https://seamplex.com/feenox/examples/neutron_diffusion.html#iaea-2d-pwr-benchmark
  [IAEA 3D PWR Benchmark]: https://seamplex.com/feenox/examples/neutron_diffusion.html#iaea-3d-pwr-benchmark
  [Cube-spherical bare reactor]: https://seamplex.com/feenox/examples/neutron_diffusion.html#cube-spherical-bare-reactor
  [Illustration of the XS dilution & smearing effect]: https://seamplex.com/feenox/examples/neutron_diffusion.html#illustration-of-the-xs-dilution-smearing-effect
  [Reed’s problem]: https://seamplex.com/feenox/examples/neutron_sn.html#reeds-problem
  [Azmy’s problem]: https://seamplex.com/feenox/examples/neutron_sn.html#azmys-problem
  [Second-order complete structured rectangular grid]: https://seamplex.com/feenox/examples/neutron_sn.html#second-order-complete-structured-rectangular-grid
  [Fist-order locally-refined unstructured triangular grid]: https://seamplex.com/feenox/examples/neutron_sn.html#fist-order-locally-refined-unstructured-triangular-grid
  [Flux profiles with ray effect]: https://seamplex.com/feenox/examples/neutron_sn.html#flux-profiles-with-ray-effect
  [RELAP7 Example 1: A simple pipe flow problem]: https://seamplex.com/feenox/examples/thermal_hydraulics.html#relap7-example-1-a-simple-pipe-flow-problem
  [RELAP7 Example 2: A transient pipe flow problem]: https://seamplex.com/feenox/examples/thermal_hydraulics.html#relap7-example-2-a-transient-pipe-flow-problem
  [RELAP7 Example 3: Adding heat to a simple pipe flow problem]: https://seamplex.com/feenox/examples/thermal_hydraulics.html#relap7-example-3-adding-heat-to-a-simple-pipe-flow-problem
  [RELAP7 Example 5: A core channel with a fuel rod]: https://seamplex.com/feenox/examples/thermal_hydraulics.html#relap7-example-5-a-core-channel-with-a-fuel-rod
  [RELAP7 Example 6: A two-pipe flow problem]: https://seamplex.com/feenox/examples/thermal_hydraulics.html#relap7-example-6-a-two-pipe-flow-problem
  [RELAP7 Example 7: A junction with multiple pipes in and out]: https://seamplex.com/feenox/examples/thermal_hydraulics.html#relap7-example-7-a-junction-with-multiple-pipes-in-and-out
  [RELAP7 Example 8: A simple pipe loop with a pump]: https://seamplex.com/feenox/examples/thermal_hydraulics.html#relap7-example-8-a-simple-pipe-loop-with-a-pump
  [RELAP7 Example 9: A heat exchanger]: https://seamplex.com/feenox/examples/thermal_hydraulics.html#relap7-example-9-a-heat-exchanger
  [RELAP7 Example 10: A loop with a core channel and a heat exchanger]: https://seamplex.com/feenox/examples/thermal_hydraulics.html#relap7-example-10-a-loop-with-a-core-channel-and-a-heat-exchanger
