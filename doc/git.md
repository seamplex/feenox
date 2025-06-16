The Git repository has the latest sources repository.
To compile, proceed as follows.
If something goes wrong and you get an error, do not hesitate to ask in FeenoX's [discussion page](https://github.com/seamplex/feenox/discussions).

> If you do not have Git or Autotools, download a [source tarball](https://seamplex.com/feenox/dist/src/) and proceed with the usual `configure` & `make` procedure. See [these instructions](doc/source.md).

:::: {.only-in-format .html }
```{=html}
<div id="feenox-clone-configure-make-check-install"></div>
<script>AsciinemaPlayer.create('feenox-clone-configure-make-check-install.cast', document.getElementById('feenox-clone-configure-make-check-install'), {cols:128,rows:32, poster: 'npt:0:10'});</script>
```
::::


 1. Install mandatory dependencies

    ```terminal
    sudo apt-get update
    sudo apt-get install git build-essential make automake autoconf libgsl-dev
    ```

    If you cannot install `libgsl-dev` but still have `git` and the build toolchain, you can have the `configure` script to download and compile it for you. See point\ 4 below.
    
 2. Install optional dependencies (of course these are _optional_ but recommended)
 
    ```terminal
    sudo apt-get install libsundials-dev petsc-dev slepc-dev
    ```

 3. Clone Github repository
 
    ```terminal
    git clone https://github.com/seamplex/feenox
    ```

 4. Bootstrap, configure, compile & make
 
    ```terminal
    cd feenox
    ./autogen.sh
    ./configure
    make -j4
    ```
    
    If you cannot (or do not want to) use `libgsl-dev` from a package repository, call `configure` with `--enable-download-gsl`:
    
    ```terminal
    ./configure --enable-download-gsl
    ```
    
    If you do not have Internet access, get the tarball manually, copy it to the same directory as `configure` and run again. See the [detailed compilation instructions](compilation.md) for an explanation.

 5. Run test suite (optional)
 
    ```terminal
    make check
    ```

 6. Install the binary system wide (optional)
 
    ```terminal
    sudo make install
    ```
    
    > If you do not have root permissions, configure with your home directory as prefix and then make install as a regular user:
    >
    > ```terminal
    > ./configure --prefix=$HOME
    > make
    > make install
    > ```
 
To stay up to date, pull and then `autogen`, `configure` and `make` (and optionally install):

```terminal
git pull
./autogen.sh
./configure
make -j4
sudo make install
```
