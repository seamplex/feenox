---
title: Distribution instructions
lang: en-US
titleblock: |
 Distribution instructions
 ========================
...

This directory contains scripts that create source and binary tarballs for FeenoX.
Also Debian `.deb` packages can be created (if running in a Debian box).

# Source tarball 

 1. Run `./src.sh` (in GNU/Linux) to get a source tarball. This step will convert the documentation from Markdown into PDF so it needs `pandoc`, `pandoc-crossref`, `xelatex`, `inkscape`, etc.

# Binary tarball

 0. Install requirements
 
    ```terminal
    apt-get install texlive-xetex texlive-fonts-extra texlive-science librsvg2-bin
    ```
    
     * Pandoc and pandoc-crossref (the versions from apt might be old)


 1. Edit `versions.sh` and set the version of both PETSc and SLEPc, for instance

    ```bash
    petsc_ver=3.20.4
    slepc_ver=3.20.1
    ```
   
 2. Run `./petsc.sh` to download, configure and compile the selected version of PETSc and SLEPc.
    These will be unpacked in the current directory `dist` but are `.gitignore`d.

 3. Run `./bin.sh` to compile FeenoX and get a tarball with a statically-linked executable.
    By default it does not include PDF documentation. Pass `--pdf` to include it:
    
    ```terminal
    ./bin.sh --pdf
    ```
    
    
Currently the supported architectures architectures are
  
 * Debian-based GNU/Linux
 * Windows with Cygwin
    
Contributions to build FeenoX in other architectures (other Unixes such as MacOS) are welcome.
 
 The resulting binary has PETSc and SLEPc statically linked, but only their non-MPI versions.
 Also, it does not include MUMPS. Feel free to play with the scripts to see if you can make them work.
 
 > To avoid generating the documentation (which needs pandoc, LaTeX, Inkscape, etc.) pass `--no-doc` to `./bin.sh`
 

# Debian packages

To build Debian packages, we need 

 1. a source tarball
 2. a proper `debian` directory
 
The first item can come from the output of `src.sh` or from <https://seamplex.com/feenox/dist/src/>.
The second from <https://salsa.debian.org/jtheler/feenox>.

Once you have both, run `dsc.sh`.


