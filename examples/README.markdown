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
5.  [Linear elasticity]
6.  [Modal analysis]
7.  [Neutron diffusion]
8.  [Neutron $S_N$]

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

All the files needed to run theses examples are available in the
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
  [Linear elasticity]: mechanical.md
  [Modal analysis]: modal.md
  [Neutron diffusion]: neutron_diffusion.md
  [Neutron $S_N$]: neutron_sn.md
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
  - [Financial decisions under inflation]
  - [The logistic map]
  - [The Fibonacci sequence]
    - [Using the closed-form formula as a function]
    - [Using a vector]
    - [Solving an iterative problem]
  - [Computing the derivative of a function as a Unix filter]
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
- Laplace’s equation
  - [How to solve a maze without AI]
    - [Transient top-down]
    - [Transient bottom-up]
  - [Potential flow around an airfoil profile]
- Heat conduction
  - [Thermal slabs]
    - [One-dimensional linear]
  - [Non-dimensional transient heat conduction on a cylinder]
  - [Non-dimensional transient heat conduction with time-dependent
    properties]
- Linear elasticity
  - [NAFEMS LE10 “Thick plate pressure” benchmark]
  - [NAFEMS LE11 “Solid Cylinder/Taper/Sphere-Temperature” benchmark]
  - [NAFEMS LE1 “Elliptical membrane” plane-stress benchmark]
  - [Parametric study on a cantilevered beam]
  - [Parallelepiped whose Young’s modulus is a function of the
    temperature]
    - [Thermal problem]
    - [Mechanical problem]
  - [Orthotropic free expansion of a cube]
  - [Thermo-elastic expansion of finite cylinders]
  - [Temperature-dependent material properties]
  - [Two cubes compressing each other]
- Mechanical modal analysis
  - [Optimizing the length of a tuning fork]
  - [Five natural modes of a cantilevered wire]
- Neutron diffusion
  - [IAEA 2D PWR Benchmark]
  - [IAEA 3D PWR Benchmark]
  - [Cube-spherical bare reactor]
  - [Illustration of the XS dilution & smearing effect]
- Neutron transport using $S_N$
  - [Reed’s problem]
  - [Azmy’s problem]
    - [Second-order complete structured rectangular grid]
    - [Fist-order locally-refined unstructured triangular grid]
    - [Flux profiles with ray effect]

  [Hello World (and Universe)!]: https://seamplex.com/feenox/examples/basic.html#hello-world-and-universe
  [Ten ways of computing *π*]: https://seamplex.com/feenox/examples/basic.html#ten-ways-of-computing-pi
  [Financial decisions under inflation]: https://seamplex.com/feenox/examples/basic.html#financial-decisions-under-inflation
  [The logistic map]: https://seamplex.com/feenox/examples/basic.html#the-logistic-map
  [The Fibonacci sequence]: https://seamplex.com/feenox/examples/basic.html#the-fibonacci-sequence
  [Using the closed-form formula as a function]: https://seamplex.com/feenox/examples/basic.html#using-the-closed-form-formula-as-a-function
  [Using a vector]: https://seamplex.com/feenox/examples/basic.html#using-a-vector
  [Solving an iterative problem]: https://seamplex.com/feenox/examples/basic.html#solving-an-iterative-problem
  [Computing the derivative of a function as a Unix filter]: https://seamplex.com/feenox/examples/basic.html#computing-the-derivative-of-a-function-as-a-unix-filter
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
  [How to solve a maze without AI]: https://seamplex.com/feenox/examples/laplace.html#how-to-solve-a-maze-without-ai
  [Transient top-down]: https://seamplex.com/feenox/examples/laplace.html#transient-top-down
  [Transient bottom-up]: https://seamplex.com/feenox/examples/laplace.html#transient-bottom-up
  [Potential flow around an airfoil profile]: https://seamplex.com/feenox/examples/laplace.html#potential-flow-around-an-airfoil-profile
  [Thermal slabs]: https://seamplex.com/feenox/examples/thermal.html#thermal-slabs
  [One-dimensional linear]: https://seamplex.com/feenox/examples/thermal.html#one-dimensional-linear
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
  [Thermo-elastic expansion of finite cylinders]: https://seamplex.com/feenox/examples/mechanical.html#thermo-elastic-expansion-of-finite-cylinders
  [Temperature-dependent material properties]: https://seamplex.com/feenox/examples/mechanical.html#temperature-dependent-material-properties
  [Two cubes compressing each other]: https://seamplex.com/feenox/examples/mechanical.html#two-cubes-compressing-each-other
  [Optimizing the length of a tuning fork]: https://seamplex.com/feenox/examples/modal.html#optimizing-the-length-of-a-tuning-fork
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
