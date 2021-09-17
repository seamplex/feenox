Browse to <https://www.seamplex.com/feenox/dist/> and check what the latest version for your architecture is. Then do

```terminal
wget https://www.seamplex.com/feenox/dist/linux/feenox-v0.1.59-gbf85679-linux-amd64.tar.gz
tar xvzf feenox-v0.1.59-gbf85679-linux-amd64.tar.gz
```

You'll have the binary under `bin` and examples, documentation, manpage, etc under `share`. Copy `bin/feenox` into somewhere in the `PATH` and that will be it. If you are root, do

```terminal
sudo cp feenox-v0.1.59-gbf85679-linux-amd64/bin/feenox /usr/local/bin
```

If you are not root, the usual way is to create a directory `$HOME/bin` and add it to your local path. If you have not done it already, do

```terminal
mkdir -p $HOME/bin
echo 'expot PATH=$PATH:$HOME/bin' >> .bashrc
```

Then finally copy `bin/feenox` to `$HOME/bin`

```terminal
cp feenox-v0.1.59-gbf85679-linux-amd64/bin/feenox $HOME/bin
```

Check if it works by calling `feenox` from any directory (you might need to open a new terminal so `.bashrc` is re-read):

```terminal
$ feenox
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
$ 
```
