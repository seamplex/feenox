Browse to <https://www.seamplex.com/feenox/dist/> and check what the latest version for your architecture is. Then do

```terminal
feenox_version=0.2.85-g48a2b76
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
FeenoX v0.2.102-g0cb44d2
a free no-fee no-X uniX-like finite-element(ish) computational engineering tool

usage: feenox [options] inputfile [replacement arguments] [petsc options]

  -h, --help         display options and detailed explanations of commmand-line usage
  -v, --version      display brief version information and exit
  -V, --versions     display detailed version information
  --pdes             list the types of PROBLEMs that FeenoX can solve, one per line

Run with --help for further explanations.
$ 
```
