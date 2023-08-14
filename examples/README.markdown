# FeenoX annotated examples

- [<span class="toc-section-number">1</span> Examples by problem type][]
- [<span class="toc-section-number">2</span> Test suite][]
- [<span class="toc-section-number">3</span> Detailed list of
  examples][]

  [<span class="toc-section-number">1</span> Examples by problem type]: #examples-by-problem-type
  [<span class="toc-section-number">2</span> Test suite]: #test-suite
  [<span class="toc-section-number">3</span> Detailed list of examples]:
    #detailed-list-of-examples

# Examples by problem type

The FeenoX examples are divided by the type of problem they solve:

1.  [Basic mathematics][]
2.  [Systems of ODEs/DAEs][]
3.  [Laplace’s equation][]
4.  [Heat conduction][]
5.  [Linear elasticity][]
6.  [Modal analysis][]
7.  [Neutron diffusion][]
8.  [Neutron $S_N$][]

> Each type of partial differential equation (i.e. from Laplace
> downward) is implemented in a subdirectory within [`src/pde`][] of the
> source tree. Feel free to…
>
> - ask questions in the [Github discussion page][]
> - propose features, corrections, improvements, etc.
> - create a pull request for other PDEs. Candidates would be
>   - fluid mechanics
>   - thermal hydraulics
>   - electromagnetism
>   - non-linear elasticity
>   - cell-level neutronics (i.e. method of characteristics, collision
>     probabilities, etc)

All the files needed to run theses examples are available in the
[examples][] directory of the [Git repository][] and any of the
[tarballs][], both [source][] and [binary][].

For a presentation of these examples, see the “FeenoX Overview
Presentation” (August 2021).

- [Recording (audio in Spanish, slides in English)][]
- [Slides in PDF][]
- [Markdown examples sources][]

  [Basic mathematics]: basic.md
  [Systems of ODEs/DAEs]: daes.md
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

The `tests` [directory in the Github repository][] has dozens of test
cases which can be used as examples for reference and for mathematical
verification of the results obtained with FeenoX. These are run when
doing `make check` after [compiling the source code][].

The test cases usually return a single number (which should be near
zero) comparing the numerical result with the expected one. Feel free to
propose benchmark problems to add to the suite.

  [directory in the Github repository]: https://github.com/seamplex/feenox/tree/main/tests
  [compiling the source code]: ../doc/compile.md

# Detailed list of examples

- Basic mathematics
  - [Hello World (and Universe)!][]
  - [Ten ways of computing *π*][]
  - [The logistic map][]
  - [The Fibonacci sequence][]
    - [Using the closed-form formula as a function][]
    - [Using a vector][]
    - [Solving an iterative problem][]
  - [Computing the derivative of a function as a UNIX filter][]
  - [On the evaluation of thermal expansion coefficients][]
- Ordinary Differential Equations & Differential-Algebraic Equations
  - [Lorenz’ attractor—the one with the butterfly][]
  - [The double pendulum][]
- Laplace’s equation
  - [How to solve a maze without AI][]
    - [Transient top-down][]
    - [Transient bottom-up][]
- Heat conduction
  - [Thermal slabs][]
    - [One-dimensional linear][]
  - [Non-dimensional transient heat conduction on a cylinder][]
  - [Non-dimensional transient heat conduction with time-dependent
    properties][]
- Linear elasticity
  - [NAFEMS LE10 “Thick plate pressure” benchmark][]
  - [NAFEMS LE11 “Solid Cylinder/Taper/Sphere-Temperature” benchmark][]
  - [NAFEMS LE1 “Elliptical membrane” plane-stress benchmark][]
  - [Parametric study on a cantilevered beam][]
  - [Parallelepiped whose Young’s modulus is a function of the
    temperature][]
    - [Thermal problem][]
    - [Mechanical problem][]
  - [Orthotropic free expansion of a cube][]
  - [Thermo-elastic expansion of finite cylinders][]
  - [Temperature-dependent material properties][]
  - [Two cubes compressing each other][]
- Mechanical modal analysis
  - [Optimizing the length of a tuning fork][]
  - [Five natural modes of a cantilevered wire][]
- Neutron diffusion
  - [IAEA 2D PWR Benchmark][]
  - [IAEA 3D PWR Benchmark][]
  - [Cube-spherical bare reactor][]
  - [Illustration of the XS dilution & smearing effect][]
- Neutron transport using $S_N$
  - [Reed’s problem][]

  [Hello World (and Universe)!]: basic.md#hello-world-and-universe
  [Ten ways of computing *π*]: basic.md#ten-ways-of-computing-pi
  [The logistic map]: basic.md#the-logistic-map
  [The Fibonacci sequence]: basic.md#the-fibonacci-sequence
  [Using the closed-form formula as a function]: basic.md#using-the-closed-form-formula-as-a-function
  [Using a vector]: basic.md#using-a-vector
  [Solving an iterative problem]: basic.md#solving-an-iterative-problem
  [Computing the derivative of a function as a UNIX filter]: basic.md#computing-the-derivative-of-a-function-as-a-unix-filter
  [On the evaluation of thermal expansion coefficients]: basic.md#on-the-evaluation-of-thermal-expansion-coefficients
  [Lorenz’ attractor—the one with the butterfly]: daes.md#lorenz-attractorthe-one-with-the-butterfly
  [The double pendulum]: daes.md#the-double-pendulum
  [How to solve a maze without AI]: laplace.md#how-to-solve-a-maze-without-ai
  [Transient top-down]: laplace.md#transient-top-down
  [Transient bottom-up]: laplace.md#transient-bottom-up
  [Thermal slabs]: thermal.md#thermal-slabs
  [One-dimensional linear]: thermal.md#one-dimensional-linear
  [Non-dimensional transient heat conduction on a cylinder]: thermal.md#non-dimensional-transient-heat-conduction-on-a-cylinder
  [Non-dimensional transient heat conduction with time-dependent properties]:
    thermal.md#non-dimensional-transient-heat-conduction-with-time-dependent-properties
  [NAFEMS LE10 “Thick plate pressure” benchmark]: mechanical.md#nafems-le10-thick-plate-pressure-benchmark
  [NAFEMS LE11 “Solid Cylinder/Taper/Sphere-Temperature” benchmark]: mechanical.md#nafems-le11-solid-cylindertapersphere-temperature-benchmark
  [NAFEMS LE1 “Elliptical membrane” plane-stress benchmark]: mechanical.md#nafems-le1-elliptical-membrane-plane-stress-benchmark
  [Parametric study on a cantilevered beam]: mechanical.md#parametric-study-on-a-cantilevered-beam
  [Parallelepiped whose Young’s modulus is a function of the temperature]:
    mechanical.md#parallelepiped-whose-youngs-modulus-is-a-function-of-the-temperature
  [Thermal problem]: mechanical.md#thermal-problem
  [Mechanical problem]: mechanical.md#mechanical-problem
  [Orthotropic free expansion of a cube]: mechanical.md#orthotropic-free-expansion-of-a-cube
  [Thermo-elastic expansion of finite cylinders]: mechanical.md#thermo-elastic-expansion-of-finite-cylinders
  [Temperature-dependent material properties]: mechanical.md#temperature-dependent-material-properties
  [Two cubes compressing each other]: mechanical.md#two-cubes-compressing-each-other
  [Optimizing the length of a tuning fork]: modal.md#optimizing-the-length-of-a-tuning-fork
  [Five natural modes of a cantilevered wire]: modal.md#five-natural-modes-of-a-cantilevered-wire
  [IAEA 2D PWR Benchmark]: neutron_diffusion.md#iaea-2d-pwr-benchmark
  [IAEA 3D PWR Benchmark]: neutron_diffusion.md#iaea-3d-pwr-benchmark
  [Cube-spherical bare reactor]: neutron_diffusion.md#cube-spherical-bare-reactor
  [Illustration of the XS dilution & smearing effect]: neutron_diffusion.md#illustration-of-the-xs-dilution-smearing-effect
  [Reed’s problem]: neutron_sn.md#reeds-problem
