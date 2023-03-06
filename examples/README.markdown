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
8.  [Neutron transport][]

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
  [Neutron transport]: neutron_transport.md
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
  - [The logistic map][]
  - [The Fibonacci sequence][]
    - [Using the closed-form formula as a function][]
    - [Using a vector][]
    - [Solving an iterative problem][]
  - [Computing the derivative of a function as a UNIX filter][]
  - [On the evaluation of thermal expansion coefficients][]
- Ordinary Differential Equations & Differential-Algebraic Equations
  - [Lorenz’ attractor—the one with the butterfly][]
- Laplace’s equation
  - [How to solve a maze without AI][]
    - [Transient top-down][]
    - [Transient bottom-up][]
- Heat conduction
  - [Thermal slabs][]
    - [One-dimensional linear][]
  - [Non-dimensional transient heat conduction on a cylinder][]
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
- Mechanical modal analysis
  - [Optimizing the length of a tuning fork][]
  - [Five natural modes of a cantilevered wire][]
- Neutron diffusion
  - [IAEA 2D PWR Benchmark][]
  - [Cube-spherical bare reactor][]
  - [Illustration of the XS dilution & smearing effect][]
- Neutron transport

  [Hello World (and Universe)!]: #hello-world-and-universe
  [The logistic map]: #the-logistic-map
  [The Fibonacci sequence]: #the-fibonacci-sequence
  [Using the closed-form formula as a function]: #using-the-closed-form-formula-as-a-function
  [Using a vector]: #using-a-vector
  [Solving an iterative problem]: #solving-an-iterative-problem
  [Computing the derivative of a function as a UNIX filter]: #computing-the-derivative-of-a-function-as-a-unix-filter
  [On the evaluation of thermal expansion coefficients]: #on-the-evaluation-of-thermal-expansion-coefficients
  [Lorenz’ attractor—the one with the butterfly]: #lorenz-attractorthe-one-with-the-butterfly
  [How to solve a maze without AI]: #how-to-solve-a-maze-without-ai
  [Transient top-down]: #transient-top-down
  [Transient bottom-up]: #transient-bottom-up
  [Thermal slabs]: #thermal-slabs
  [One-dimensional linear]: #one-dimensional-linear
  [Non-dimensional transient heat conduction on a cylinder]: #non-dimensional-transient-heat-conduction-on-a-cylinder
  [NAFEMS LE10 “Thick plate pressure” benchmark]: #nafems-le10-thick-plate-pressure-benchmark
  [NAFEMS LE11 “Solid Cylinder/Taper/Sphere-Temperature” benchmark]: #nafems-le11-solid-cylindertapersphere-temperature-benchmark
  [NAFEMS LE1 “Elliptical membrane” plane-stress benchmark]: #nafems-le1-elliptical-membrane-plane-stress-benchmark
  [Parametric study on a cantilevered beam]: #parametric-study-on-a-cantilevered-beam
  [Parallelepiped whose Young’s modulus is a function of the temperature]:
    #parallelepiped-whose-youngs-modulus-is-a-function-of-the-temperature
  [Thermal problem]: #thermal-problem
  [Mechanical problem]: #mechanical-problem
  [Orthotropic free expansion of a cube]: #orthotropic-free-expansion-of-a-cube
  [Thermo-elastic expansion of finite cylinders]: #thermo-elastic-expansion-of-finite-cylinders
  [Temperature-dependent material properties]: #temperature-dependent-material-properties
  [Optimizing the length of a tuning fork]: #optimizing-the-length-of-a-tuning-fork
  [Five natural modes of a cantilevered wire]: #five-natural-modes-of-a-cantilevered-wire
  [IAEA 2D PWR Benchmark]: #iaea-2d-pwr-benchmark
  [Cube-spherical bare reactor]: #cube-spherical-bare-reactor
  [Illustration of the XS dilution & smearing effect]: #illustration-of-the-xs-dilution-smearing-effect
