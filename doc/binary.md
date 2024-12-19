Browse to <https://www.seamplex.com/feenox/dist/> and check what the latest version for your architecture is. Then do

```terminal
feenox_version=1.0.8
wget -c https://www.seamplex.com/feenox/dist/linux/feenox-v${feenox_version}-linux-amd64.tar.gz
tar xzf feenox-v${feenox_version}-linux-amd64.tar.gz
sudo cp feenox-v${feenox_version}-linux-amd64/bin/feenox /usr/local/bin
```

You'll have the binary under `bin` and examples, documentation, manpage, etc under `share`. Copy `bin/feenox` into somewhere in the `PATH` and that will be it. If you are root, do

```terminal
sudo cp feenox-v${feenox_version}-linux-amd64/bin/feenox /usr/local/bin
```

If you are not root, the usual way is to create a directory `$HOME/bin` and add it to your local path. If you have not done it already, do

```terminal
mkdir -p $HOME/bin
echo 'expot PATH=$PATH:$HOME/bin' >> .bashrc
```

Then finally copy `bin/feenox` to `$HOME/bin`

```terminal
cp feenox-v${feenox_version}-linux-amd64/bin/feenox $HOME/bin
```

Check if it works by calling `feenox` from any directory (you might need to open a new terminal so `.bashrc` is re-read):

```terminal
$ feenox
FeenoX v1.0.8-g731ca5d 
a cloud-first free no-fee no-X uniX-like finite-element(ish) computational engineering tool

usage: ./feenox [options] inputfile [replacement arguments] [petsc options]

  -h, --help         display options and detailed explanations of command-line usage
  -v, --version      display brief version information and exit
  -V, --versions     display detailed version information
  -c, --check        validates if the input file is sane or not
  --pdes             list the types of PROBLEMs that FeenoX can solve, one per line
  --elements_info    output a document with information about the supported element types
  --linear           force FeenoX to solve the PDE problem as linear
  --non-linear       force FeenoX to solve the PDE problem as non-linear

Run with --help for further explanations.
$ 
```
