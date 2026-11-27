---
name: feenox
description: "How to write FeenoX input files (*.fee) and run FeenoX from a script: the input syntax, problem types, meshes from Gmsh, materials and boundary conditions, integrals and extrema, printing results for a script to parse. Includes FeenoX's complete reference, one file per keyword, function and variable, and its examples and tests grouped by problem type. Read before writing or changing any FeenoX input."
---

# Writing FeenoX inputs

FeenoX is a finite-element tool driven by a plain-text input file, a
"syntactically sugared" description of the problem. A script or a person
writes `<name>.fee`, runs `feenox <name>.fee`, and reads what it prints or
writes.

**Never write a keyword, option, function or variable from memory.** FeenoX is
much less common than Python or Gmsh, and plausible-looking syntax is often
wrong. Look each one up in the files that come with this skill (paths are
relative to this file's directory):

- `syntax.md`: the input-file syntax (lines, comments, quoting, expressions,
  boundary conditions) and guidance for generated input files. Read it first.
- `reference/INDEX.md`: every entry with a one-line summary and its file.
- `reference/keywords/<KEYWORD>.md`: e.g. `reference/keywords/PRINT.md`,
  `reference/keywords/INTEGRATE.md`, `reference/keywords/PROBLEM.md`.
- `reference/functions/<name>.md`, `reference/functionals/`,
  `reference/vector-functions/`, `reference/variables/` (special variables,
  e.g. `done`, `t`).
- `reference/problems/<problem>/`: for each problem type, its
  `boundary-conditions/`, `properties/` (material properties), `results/`
  (the fields it computes), `variables/` and, for some, `keywords/`. E.g.
  `reference/problems/thermal/properties/k.md`.
- `EXAMPLES.md`: FeenoX's examples and tests, grouped by `PROBLEM`, each with
  the keywords it uses and the Gmsh `.geo` of its mesh. The files are in
  `examples/` and `tests/`, and every reference entry lists some that use it.
  **Start a new input from an example with the same `PROBLEM`**, rather than
  from scratch.
- `command-line.md`: FeenoX's command-line options.

This skill was generated from the documentation of FeenoX `VERSION`. If
`feenox --version` reports another version, say so: some entries may differ.

## Essentials

- Keywords are either instructions (verbs, e.g. `PRINT`, `SOLVE_PROBLEM`),
  executed in order, or definitions (nouns, e.g. `FUNCTION`, `MATERIAL`).
- Everything is an expression. Wherever a number is needed, an algebraic
  expression can be written.
- One instruction per line; `#` starts a comment (inside a string it
  must be escaped: `"\#"`). Write keywords in uppercase; names of variables,
  functions, properties and boundary conditions are case-sensitive.
- **A misspelled name is not an error by default:** `PRINT Vol` with no `Vol`
  defined prints the text `Vol`. Have the script check what it reads, or put
  `IMPLICIT NONE` at the top, which makes unknown names errors (then every
  variable needs `VAR <name>` before it is assigned).
- **FeenoX has no units.** Use one consistent system (SI unless told
  otherwise) and write the unit in a comment next to each value.
- Assignments: `k = 237` defines a variable; `f(x,y,z) = ...` a function.
- Values can come from the command line as `$1`, `$2`, ... (see
  "Command-line arguments" below).
- A PDE is solved with the `PROBLEM` keyword. Available problems are
   * `laplace` for the Laplace or Poisson equation
   * `thermal` for heat conduction
   * `mechanical` for elasticity
   * `modal` for modal mechanical analysis
   * `neutron_diffusion` for core-level multi-group neutron diffusion
   * `neutron_sn` for core-level multi-group discrete ordinates neutron transport
   * `thermal_hydraulics` for one-dimensional thermal hydraulics (like RELAP)

  Give the dimensions in `PROBLEM` (e.g. `PROBLEM thermal 3D`) whenever a
  property or condition is a function of space. `feenox --pdes` lists the
  problems compiled into the binary at hand.
- The mesh has to be in Gmsh `.msh` format (versions 2.2, 4.0, 4.1 or 4.2,
  ASCII or binary) and can be given either by the secondary keyword `MESH`
  in the `PROBLEM` definition or by a stand-alone `READ_MESH` instruction.
- To solve a PDE the input usually gives
  * material properties (see below)
  * boundary conditions
  * the `SOLVE_PROBLEM` instruction. If it is missing, FeenoX solves the
    problem the first time a result (e.g. `T(0,0,0)`) is needed. Write it
    explicitly anyway, before the instructions that use the solution; there
    can be only one. In transient problems always write it: without it,
    the time stepping can come out different.
  * output instructions such as `WRITE_RESULTS`, `PRINT` or `PRINTF`
- Materials and boundary conditions refer to Gmsh **physical groups by
  name**: `MATERIAL <group> <property>=<expr>` and
  `BC <group> <condition>=<expr>` (see `reference/keywords/MATERIAL.md` and
  `reference/keywords/BC.md`). The names of properties and conditions depend
  on the problem type: look them up under `reference/problems/<problem>/`.
- Integrals and extrema over a physical group:
  `INTEGRATE <expr> OVER <group> RESULT <variable>`,
  `FIND_EXTREMA <expr> OVER <group> MIN <var> MAX <var>`. But first check
  whether the problem already computes what you need (see "Results" below).

## Material properties

A property can be given in three equivalent ways:

```text
# 1. one MATERIAL per volumetric physical group
MATERIAL mat1 k=1
MATERIAL mat2 k=1+x^2+y^2+z^2

# 2. variables or functions named <property>_<group>
k_mat1(x,y,z) = 1
k_mat2(x,y,z) = 1+x^2+y^2+z^2

# 3. a single variable or function for the whole domain
k = 1
```

- With the second form, each group can use either a variable or a
  function: `k_mat1 = 1` together with `k_mat2(x,y,z) = 1+x` works.
- Functions of space take as many arguments as the problem has dimensions
  (`(x,y)` in 2D, `(x,y,z)` in 3D).
- Every volumetric physical group needs every property the problem uses.

## Results

Once solved, each problem defines **result functions** of space and
**special variables** with derived results. Look them up for your problem
before computing anything yourself:

- `reference/problems/<problem>/results/`: the fields, e.g. `T`, `qx`, `qy`,
  `qz` (thermal); `u`, `v`, `w`, the strains `exx` ..., the stresses
  `sigmax`, `sigmay`, `sigmaz`, `tauxy` ..., the principal stresses
  `sigma1`, `sigma2`, `sigma3`, the von Mises stress `sigma` and `tresca`
  (mechanical); `phi1`, `phi2` ... per group (neutron problems: there is
  no `phi`); the mode shapes `u1`, `v1`, `w1`, `u2` ... (modal).
- `reference/problems/<problem>/variables/`: scalars and vectors computed
  after solving, e.g. `T_max` and `T_min` (thermal); `displ_max` and its
  location `displ_max_x` ..., `sigma_max` (the maximum von Mises stress over
  the nodes) and its location, `strain_energy` (mechanical); `keff` (neutron problems);
  the vectors of frequencies `f(i)` and `omega(i)` (modal).
- The derivatives of the primary unknowns are functions named
  `d<unknown>d<x|y|z>`, e.g. `dTdx`, `dudz`, `dphi1dy`.

So the maximum displacement and the maximum von Mises stress of a
mechanical problem are just

```text
PRINT %.6e displ_max sigma_max
```

with no need for `FIND_EXTREMA` over `sigma`. These maxima (and `T_max`,
`T_min`) are taken over the nodes, i.e. they equal
`FIND_EXTREMA sigma MAX s NODES`. `FIND_EXTREMA` without `NODES`, `CELLS` or
`GAUSS` searches the nodes, the cells and the Gauss points, so it may give a
slightly different value. Use the result functions
evaluated at points (`T(0,0,0)`), in `INTEGRATE` and `FIND_EXTREMA` over a
physical group, or in `WRITE_RESULTS` for the whole field. Remember that a
misspelled or non-existent name (e.g. `phi` instead of `phi1`) is printed
as text, not reported as an error.

## Printing results for a script

`PRINT` tries to parse each argument it sees into a valid expression.
If it is, it is printed. If not, the text is printed. If one wants to
print the argument verbatim even if it is a valid expression, prepend
`TEXT` to the argument (it applies to that one argument only). Each argument is a token so if you want to use spaces
in an expression, double-quote it. If any of the arguments starts with a `%`
it is read as a printf format for double-precision arguments. All the arguments
after the `%` use the prescribed format. If no format is given, default is `%g`.
By default the arguments are separated by a tab. The separator can be
changed by passing `SEP`.

```text
# three numbers separated by tabs on one line
PRINT V m_al m_au
# first number is %.3f and the last two are %.2f
PRINT %.3f V %.2f m_al m_au

# this prints 2
PRINT 1+1

# this also prints 2
PRINT "1+1"

# this prints 1\t+\t1
PRINT 1 + 1

# this prints "1 + 1"
PRINT TEXT "1 + 1"

# this also prints "1 + 1": TEXT keeps the first 1, + is not an expression
# so it is text, and SEP " " separates with a space instead of a tab
PRINT TEXT 1 + 1 SEP " "
```

There is also a `PRINTF` instruction that works like C's `printf`: the
first token is a string with a mixture of verbatim characters and format
specifiers, followed by one expression per specifier. It adds a newline at
the end.

```text
# this also prints "1 + 1"
PRINTF "%g + %g" 1 1
```

- Print exactly what the script parses, and have the script check it (the
  number of values, that each is a number) before using it.
- For fields (temperatures, stresses over the mesh), write a results file
  with `WRITE_RESULTS FORMAT vtu` (or `gmsh`).

## A complete example

The volume of a unit cube:

```text
READ_MESH cube.msh                  # from Gmsh: Physical Volume("solid")
INTEGRATE 1 OVER solid RESULT V     # m^3
PRINT %.3f V
```

prints `1.000`.

## Command-line arguments

Everything after the input file on the command line that does not start
with a hyphen is an argument for the input: FeenoX replaces each `$1`, `$2`,
... (also `${1}`, and `$10` and beyond) in the input with the first, second,
... argument, verbatim, as text, before parsing the line. So an argument
can be a number, a part of an expression, or a part of a file name:

```text
# run as: feenox cube.fee 2 fine
READ_MESH cube-$2.msh                  # reads cube-fine.msh
k = $1                                 # k = 2
PRINT "result for mesh $2:" k
```

- A `$n` without its argument is an error ("input file needs at least one
  more argument in commandline"), unless the input gives it a default with
  `DEFAULT_ARGUMENT_VALUE <n> <value>`, e.g. `DEFAULT_ARGUMENT_VALUE 2 coarse`.
  Comments are not expanded, so a `$3` in a comment needs no argument.
- An argument that starts with a hyphen is taken as an option, so
  `feenox model.fee -1` fails. Pass a negative number in parentheses:
  `feenox model.fee "(-1)"`.
- An argument with spaces expands into several tokens. Quote it in the input
  (`"$1"`) when it has to stay one expression.
- Use arguments to run the same input for several values (a parametric
  study, mesh refinements) instead of writing one input per value; to pass
  options to PETSc, see `command-line.md`.

## Running it from a script

```python
out = subprocess.run(["feenox", "model.fee", str(k), "fine"], cwd=workdir, check=True,
                     capture_output=True, text=True).stdout
version = subprocess.run(["feenox", "--version"], capture_output=True,
                         text=True).stdout.split("\n")[0]       # e.g. "FeenoX v1.3"
```

Record the version with the results.

## Before running or proposing an input

- Look up every keyword, property and boundary condition you use.
- Check it with `feenox --check model.fee` when FeenoX is available, then run
  the smallest representative case.
- Say what you could not check.
