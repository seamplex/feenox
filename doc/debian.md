Debian/Ubuntu packages are available at <https://www.seamplex.com/feenox/dist/>.
Find the directory for your Debian or Ubuntu release, i.e.

 * [`bookworm`](https://www.seamplex.com/feenox/dist/deb/bookworm) is Debian 12
 * [`bullseye`](https://www.seamplex.com/feenox/dist/deb/bullseye) is Debian 11
 * [`buster`](https://www.seamplex.com/feenox/dist/deb/buster) is Debian 10
 * [`kinetic`](https://www.seamplex.com/feenox/dist/deb/kinetic) is Ubuntu 22.10
 * [`jammy`](https://www.seamplex.com/feenox/dist/deb/jammy) is Ubuntu 22.04
 * [`focal`](https://www.seamplex.com/feenox/dist/deb/focal) is Ubuntu 20.04
 
If you know how to install `.deb` packages, feel free to use your method (i.e. `gdebi` or with the "Software Center" thing).

You can can always use `dpkg` (as root):

```terminal
$ sudo dpkg -i feenox-0.3_1_amd64.deb`
```

Most likely, this step will fail due to failed dependencies.
Just call `apt` to fix them for you:

```terminal
$ sudo apt-get --fix-broken install
```

Now the command `feenox` should be globally available:

```terminal
$ feenox
FeenoX v0.2.219-g9e9ef22 
a cloud-first free no-fee no-X uniX-like finite-element(ish) computational engineering tool

usage: feenox [options] inputfile [replacement arguments] [petsc options]

  -h, --help         display options and detailed explanations of commmand-line usage
  -v, --version      display brief version information and exit
  -V, --versions     display detailed version information
  --pdes             list the types of PROBLEMs that FeenoX can solve, one per line

Run with --help for further explanations.
$
```

If the execution fails, most likely the version of the `.deb` does not match your GNU/Linux release.
Please try the statically-linked binaries below or ask in the [FeenoX discussions](https://github.com/seamplex/feenox/discussions) page.

The FeenoX Unix man page should be available in section one:

```terminal
$ man -k feenox
feenox (1)           - a cloud-first free no-X uniX-like finite-element(ish) computational engineering tool
$ man feenox
$
```
