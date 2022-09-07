---
subtitle: "FeenoX Tutorial \\#0"
title: Setting up your workspace
---

- [<span class="toc-section-number">1</span> Foreword][]
- [<span class="toc-section-number">2</span> FeenoX][]
  - [<span class="toc-section-number">2.1</span> Compiling from
    source][]
  - [<span class="toc-section-number">2.2</span> Executing an example][]
  - [<span class="toc-section-number">2.3</span> Executing a test][]
- [<span class="toc-section-number">3</span> Gmsh][]
- [<span class="toc-section-number">4</span> Text editor][]
  - [<span class="toc-section-number">4.1</span> Vim][]
  - [<span class="toc-section-number">4.2</span> Kate][]
- [<span class="toc-section-number">5</span> Post-processors][]
- [<span class="toc-section-number">6</span> Other UNIX tools][]
  - [<span class="toc-section-number">6.1</span> Awk][]
  - [<span class="toc-section-number">6.2</span> M4][]

  [<span class="toc-section-number">1</span> Foreword]: #foreword
  [<span class="toc-section-number">2</span> FeenoX]: #feenox
  [<span class="toc-section-number">2.1</span> Compiling from source]: #compiling-from-source
  [<span class="toc-section-number">2.2</span> Executing an example]: #executing-an-example
  [<span class="toc-section-number">2.3</span> Executing a test]: #executing-a-test
  [<span class="toc-section-number">3</span> Gmsh]: #gmsh
  [<span class="toc-section-number">4</span> Text editor]: #text-editor
  [<span class="toc-section-number">4.1</span> Vim]: #vim
  [<span class="toc-section-number">4.2</span> Kate]: #kate
  [<span class="toc-section-number">5</span> Post-processors]: #post-processors
  [<span class="toc-section-number">6</span> Other UNIX tools]: #other-unix-tools
  [<span class="toc-section-number">6.1</span> Awk]: #awk
  [<span class="toc-section-number">6.2</span> M4]: #m4

# Foreword

[FeenoX][] is a cloud-first engineering tool. Therefore, it runs
natively on GNU/Linux platforms. Theoretically, the tool could be
compiled and run in other architectures such as Windows or MacOS in a
non-cloud approach, although this is discouraged because those two
operating systems are

1.  not cloud-friendly, let alone cloud-first; and
2.  neither free-as-in-free-beer nor open source.

In order to take the tutorials that follow, it is then recommended to
stick to GNU/Linux as explained below.

The best way to learn and to understand how FeenoX works is to use a
native GNU/Linux distribution as the main operating system, either in a
laptop or a desktop PC. This sentence from PETSc’s Matt Kneppley from
2015 speaks for itself:

> “It is really worth any amount of time and effort to get away from
> Windows if you are doing computational science.”
>
> <https://lists.mcs.anl.gov/pipermail/petsc-users/2015-July/026388.html>

If you already use GNU/Linux any distribution will do, although FeenoX
is developed in Debian so `apt` (or `apt-get`) will be used as the
package manager. If you do not currently use GNU/Linux as your main
operating system and still do not want to spend any time nor effort on
doing things right, you can either

1.  fire up a virtual GNU/Linux server in a cloud provider (e.g. AWS,
    Azure, DigitalOcean, Contabo, etc.) and connect through SSH
2.  use a containerized GNU/Linux (e.g. with `docker`)
3.  use a GNU/Linux box through an virtual computer emulator
    (e.g. VirtualBox, VMWare, Vagrant, etc.)

Up to this point, I assume you have access to a shell as a regular user
and that you have permissions to use `sudo`. If you do not know what
this means, look it up in your favorite search engine or ask for help.

In the following sections there will be terminal mimics. Lines starting
with `$` show commands that ought to be typed into the command line. The
`$` itself does not have to be typed as it is part of the prompt. Lines
not starting with a dollar sign show the output of the invoked command.

  [FeenoX]: https://www.seamplex.com/feenox

# FeenoX

The most important thing to be set up is FeenoX itself. Since there are
still no Debian packages for FeenoX, the most straightforward way to go
is to download the Linux binary tarball from
<https://seamplex.com/feenox/dist/linux/> and copy the executable into
`/usr/local/bin` so it is globally available. To download the tarball
you need `wget` and to un-compress it `tar` and `gz` (which should be
already installed anyway), so do

``` terminal
$ sudo apt-get install wget tar gzip
$ wget https://seamplex.com/feenox/dist/linux/feenox-v0.2.85-g48a2b76-linux-amd64.tar.gz
$ tar xvzf feenox-v0.2.85-g48a2b76-linux-amd64.tar.gz
$ sudo cp feenox-v0.2.85-g48a2b76-linux-amd64/bin/feenox /usr/local/bin/
```

You should now be able to invoke FeenoX by executing `feenox` from any
directory. See the “Invocation” section of the [FeenoX Manual][] for
details about how to invoke it. Check this is the case:

``` terminal
$ feenox
FeenoX v0.2.114-g45b8799-dirty 
a free no-fee no-X uniX-like finite-element(ish) computational engineering tool

usage: feenox [options] inputfile [replacement arguments] [petsc options]

  -h, --help         display options and detailed explanations of commmand-line usage
  -v, --version      display brief version information and exit
  -V, --versions     display detailed version information
  --pdes             list the types of PROBLEMs that FeenoX can solve, one per line

Run with --help for further explanations.
$
```

If you get an error, plase ask for help in the [Github discussion
page][].

  [FeenoX Manual]: https://www.seamplex.com/feenox/doc/feenox-manual.html#invocation
  [Github discussion page]: https://github.com/seamplex/feenox/discussions

## Compiling from source

If you want to tweak the compilation flags, use other libraries, modify
the code or just learn how FeenoX works, read the [Compilation
instructions][].

  [Compilation instructions]: https://www.seamplex.com/feenox/doc/compilation.html

## Executing an example

Find the `examples` directory and go into it. If you downloaded the
binary tarball, it will be in `share/doc/examples`. If you downloaded
the source tarball or cloned the repository, it will directly
`examples`.

``` terminal
cd share/doc/examples
feenox 
```

## Executing a test

# Gmsh

To solve problems involving partial differential equations
(i.e. elasticity, heat conduction, etc.) FeenoX needs a mesh in [Gmsh’s
MSH format][]. Any mesher whose output format can be converted to `.msh`
should work, altough of course the most natural way to create these
meshes is to use [Gmsh][] itself.

The easiest way to go is to install Gmsh from the `apt` repository:

``` terminal
$ sudo apt-get install gmsh
```

Check `gmsh` is globally available by calling it with `-info`:

``` terminal
$ gmsh -info
Version       : 4.10.5
License       : GNU General Public License
Build OS      : Linux64
Build date    : 20220701
Build host    : gmsh.info
Build options : 64Bit ALGLIB[contrib] ANN[contrib] Bamg Blas[petsc] Blossom Cgns DIntegration Dlopen DomHex Eigen[contrib] Fltk Gmm[contrib] Hxt Jpeg Kbipack Lapack[petsc] LinuxJoystick MathEx[contrib] Med Mesh Metis[contrib] Mmg Mpeg Netgen ONELAB ONELABMetamodel OpenCASCADE OpenCASCADE-CAF OpenGL OpenMP OptHom PETSc Parser Plugins Png Post QuadMeshingTools QuadTri Solver TetGen/BR Voro++[contrib] WinslowUntangler Zlib
FLTK version  : 1.4.0
PETSc version : 3.14.4 (real arithmtic)
OCC version   : 7.6.1
MED version   : 4.1.0
Packaged by   : geuzaine
Web site      : https://gmsh.info
Issue tracker : https://gitlab.onelab.info/gmsh/gmsh/issues
$
```

It should be noted that depending on the version of the base opearating
system, the Gmsh version in the `apt` repository might be old enough so
as to fail with the examples provided in the tutorials, that are based
on recent Gmsh versions. If this is the case, as with FeenoX above, you
can always [download newer Gmsh binaries][]:

``` terminal
$ wget http://gmsh.info/bin/Linux/gmsh-4.10.5-Linux64.tgz
$ tar gmsh-4.10.5-Linux64.tgz
$ sudo cp gmsh-4.10.5-Linux64/bin/gmsh /usr/local/bin
```

Also, Gmsh can be compiled from source by following the [instructions in
the documentation][].

It is important to note that Gmsh creates its meshes by reading an input
file with extension `.geo` without needing to use a graphical interface
at all. Therefore, Gmsh can be used even through `ssh` or `docker` where
there is no graphical device. However, it does provide a GUI that is
very handy to create the `.geo` in the first place and to help identify
the ids of the surfaces that will be subject to boundary conditions. So
you will be able to go through the tutorials in text-only mode but you
will not be able to exploit its full potential nor even see what the
mesh you are using look like.

  [Gmsh’s MSH format]: http://gmsh.info/doc/texinfo/gmsh.html#MSH-file-format
  [Gmsh]: http://gmsh.info/
  [download newer Gmsh binaries]: http://gmsh.info/#Download
  [instructions in the documentation]: http://gmsh.info/doc/texinfo/gmsh.html#Compiling-the-source-code

# Text editor

In order to create the both the FeenoX and the Gmsh input files, you
will need a text editor. In principle any editor will do, but since
FeenoX uses keywords to define the problem being solved, it is way more
efficient (and aesthetically pleasant) to use one that supports syntax
highlighting.

## Vim

The `vim` text editor can be perfectly used to edit input files. It is
text-only so it works through `ssh` and `docker`. It has a non-trivial
learning curve but it is worth to learn its basics because at some point
you will want to run FeenoX in an actual cloud server and [Vim][] will
be your friend there.

To enable syntax highlighting copy the file `fee.vim` into
`~/.vim/syntax`.

![FeenoX input file edited in Vim on Konsole][]

  [Vim]: https://www.vim.org/
  [FeenoX input file edited in Vim on Konsole]: highlighting-vim.png

## Kate

For those using a native GNU/Linux box with a graphical interface, the
recommended editor is [Kate][].

![FeenoX input file edited in Kate on Plasma][]

  [Kate]: https://kate-editor.org/
  [FeenoX input file edited in Kate on Plasma]: highlighting-kate.png

# Post-processors

FeenoX can write mesh results either in `.msh` or `.vtk` format. The
former can be read and postprocessed by Gmsh. The latter can be read and
postprocessed by a few different tools, but Paraview is the flagship
postprocessor. In general any version will do, so it can be installed
with

``` terminal
$ sudo apt-get install paraview
```

Also binaries and source versions can be [downloaded][].

Note that both Gmsh in post-processing mode and Paraview make sense only
if you have access to a graphical device.

  [downloaded]: https://www.paraview.org/download/

# Other UNIX tools

## Awk

## M4