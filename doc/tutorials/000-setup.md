---
title: Setting up your workspace
subtitle: FeenoX Tutorial #0
...

Once again, [FeenoX](https://www.seamplex.com/feenox) is a cloud-first engineering tool. Therefore, it runs natively on GNU/Linux platforms. Theoretically, the tool could be compiled and run in other architectures such as Windows or MacOS in a non-cloud approach, although this is discouraged because those two operating systems are

 1. not cloud-friendly, let alone cloud-first; and
 2. neither free-as-in-free-beer nor open source.
 
In order to take the tutorials that follow, it is then recommended to stick to GNU/Linux as explained below.

The best way to learn and to understand how FeenoX works is to use a native GNU/Linux distribution as the main operating system, either in a laptop or a desktop PC. This sentence from PETSc's Matt Kneppley from 2015 speaks for itself:

> “It is really worth any amount of time and effort to get away from Windows if you are doing computational science.”
>
> <https://lists.mcs.anl.gov/pipermail/petsc-users/2015-July/026388.html>



If you already use GNU/Linux any distribution will do, although FeenoX is developed in Debian so `apt` (or `apt-get`) will be used as the package manager.
If you do not currently use GNU/Linux as your main operating system and still do not want to spend any time nor effort on doing things right, you can either

 i. fire up a virtual GNU/Linux server in a cloud provider (e.g. AWS, Azure, DigitalOcean, Contabo, etc.) and connect through SSH
 ii. use a containerized GNU/Linux (e.g. with `docker`)
 iii. use a GNU/Linux box through an virtual computer emulator (e.g. VirtualBox, VMWare, Vagrant, etc.)


Up to this point, I assume you have access to a shell as a regular user and that you have permissions to use `sudo`. If you do not know what this means, look it up in your favorite search enginer.
In the following sections there will be terminal mimics. Lines starting with `$` show commands that ought to be typed into the command line. The `$` itself does not have to be typed as it is part of the prompt. Lines not starting with a dollar sign show the output of the invoked command.


# FeenoX

The most important thing to be set up is FeenoX itself. Since there are still no Debian packages for FeenoX, the most straighforward way to go is to download the Linux binary tarball from https://seamplex.com/feenox/dist/linux/ and copy the executable into `/usr/local/bin` so it is globally available. To download the tarball you need `wget` and to uncompress it `tar` and `gz` (which should be already installed anyway), so do

```
$ sudo apt-get install wget tar gzip
$ wget https://seamplex.com/feenox/dist/linux/feenox-v0.2.85-g48a2b76-linux-amd64.tar.gz
$ tar xvzf feenox-v0.2.85-g48a2b76-linux-amd64.tar.gz
$ sudo cp feenox-v0.2.85-g48a2b76-linux-amd64/bin/feenox /usr/local/bin/
```

You should now be able to invoke FeenoX by executing `feenox` from any directory. See the [“Invocation”] section of the [FeenoX Manual](https://www.seamplex.com/feenox/doc/feenox-manual.html#invocation) for details about how to invoke it.
Check this is the case:

```
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

If you get an error, plase ask for help in the [Github discussion page](https://github.com/seamplex/feenox/discussions).


## Compiling from source

If you want to tweak the compilation flags, use other libraries, modify the code or just learn how FeenoX works, read the [Compilation instructions](https://www.seamplex.com/feenox/doc/compilation.html).


# Gmsh

To solve problems involving partial differential equations (i.e. elasticity, heat conduction, etc.) FeenoX needs a mesh in [Gmsh's MSH format](http://gmsh.info/doc/texinfo/gmsh.html#MSH-file-format). Any mesher whose output format can be converted to `.msh` should work, altough of course the most natural way to create these meshes is to use [Gmsh](http://gmsh.info/) itself.

The easiest way to go is to install Gmsh from the `apt` repository:

```
$ sudo apt-get install gmsh
```

Check `gmsh` is globally available by calling it with `-info`:

```
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

It should be noted that depending on the version of the base opearating system, the Gmsh version in the `apt` repository might be old enough so as to fail with the examples provided in the tutorials, that are based on recent Gmsh versions. If this is the case, as with FeenoX above, you can always [download newer Gmsh binaries](http://gmsh.info/#Download):

```
$ wget http://gmsh.info/bin/Linux/gmsh-4.10.5-Linux64.tgz
$ tar gmsh-4.10.5-Linux64.tgz
$ sudo cp gmsh-4.10.5-Linux64/bin/gmsh /usr/local/bin
```

Also, Gmsh can be compiled from source by following the [instructions in the documentation](http://gmsh.info/doc/texinfo/gmsh.html#Compiling-the-source-code).


It is important to note that Gmsh creates its meshes by reading an input file with extension `.geo` without needing to use a graphical interface at all. Therefore, Gmsh can be used even through `ssh` or `docker` where there is no graphical device. However, it does provide a GUI that is very handy to create the `.geo` in the first place and to help identify the ids of the surfaces that will be subject to boundary conditions. So you will be able to go through the tutorials in text-only mode but you will not be able to exploit its full potential nor even see what the mesh you are using look like.


# Text editor

In order to create the both the FeenoX and the Gmsh input files, you will need a text editor. In principle any editor will do, but since FeenoX uses keywords to define the problem being solved, it is way more efficient (and aesthetically pleasant) to use one that supports syntax highlighting.

## Vim

The `vim` text editor can be perfectly used to edit input files. It is text-only so it works through `ssh` and `docker`.
It has a non-trivial learning curve but it is worth to learn its basics because at some point you will want to run FeenoX in an actual cloud server and [Vim](https://www.vim.org/) will be your friend there.

To enable syntax highlighting copy the file `fee.vim` into `~/.vim/syntax`.

![FeenoX input file edited in Vim on Konsole](../highlighting-vim.png)



## Kate

For those using a native GNU/Linux box with a graphical interface, the recommended editor is [Kate](https://kate-editor.org/).

![FeenoX input file edited in Kate on Plasma](../highlighting-kate.png)

