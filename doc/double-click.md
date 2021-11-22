# Why nothing happens when I double click on `feenox.exe`?

Because by design, FeenoX does not have a Graphical User Interface. Instead, it works like a transfer function between one or more input files and zero or more output files:

```include
transfer.md
```

Recall that FeenoX is designed as a cloud-first tool, and “the cloud” runs on UNIX (essentially GNU/Linux) so FeenoX is based on the [UNIX philosophy](unix.md). In this world, programs act like filters (i.e. transfer functions). They are executed from a command-line terminal. So instead of “double clicking” the executable, one has to open a terminal and execute it there. Without any arguments it says how to use it:

```terminal
> feenox.exe
FeenoX v0.1.67-g8899dfd-dirty 
a free no-fee no-X uniX-like finite-element(ish) computational engineering tool

usage: feenox [options] inputfile [replacement arguments]

  -h, --help         display usage and commmand-line help and exit
  -v, --version      display brief version information and exit
  -V, --versions     display detailed version information
  -s, --sumarize     list all symbols in the input file and exit

Instructions will be read from standard input if “-” is passed as
inputfile, i.e.

    $ echo "PRINT 2+2" | feenox -
    4

Report bugs at https://github.com/seamplex/feenox or to jeremy@seamplex.com
Feenox home page: https://www.seamplex.com/feenox/
>
```

It is explained there that the main input file has to be given as the first argument. So go to the tests or examples directory, find a test you like and run it:

```terminal
> cd tests
> feenox parallelepiped.fee
0.000295443
>
```

In any case, recall once again that FeenoX is a cloud-first tool, and Windows is not cloud-friendly, let alone cloud-first.
It is time to re-think what you expect from a finite-element(ish) tool.
If you still need a GUI, please check [CAEplex](https://www.caeplex.com).

Try to avoid Windows as much as you can. The binaries are provided as transitional packages for people that for some reason still use such an outdated, anachronous, awful and invasive operating system. They are compiled with [Cygwin](http://cygwin.com/) and have no support whatsoever. Really, really, **get rid of Windows ASAP**.

```include
windows.md
```
