To compile the source tarball, proceed as follows. This procedure does not need `git` nor `autoconf` but a new tarball has to be downloaded each time there is a new FeenoX version.

 1. Install mandatory dependencies

    ```terminal
    sudo apt-get update
    sudo apt-get install gcc make libgsl-dev
    ```

    If you cannot install `libgsl-dev`, you can have the `configure` script to download and compile it for you. See point\ 4 below.
    
 2. Install optional dependencies (of course these are _optional_ but recommended)
 
    ```terminal
    sudo apt-get install libsundials-dev petsc-dev slepc-dev
    ```

 3. Download and uncompress FeenoX source tarball. Browse to <https://www.seamplex.com/feenox/dist/src/> and pick the latest version:
 
 
    ```terminal
    wget https://www.seamplex.com/feenox/dist/src/feenox-v0.1.66-g1c4b17b.tar.gz
    tar xvzf feenox-v0.1.66-g1c4b17b.tar.gz
    ```
 
 4. Configure, compile & make
 
    ```terminal
    cd feenox-v0.1.66-g1c4b17b
    ./configure
    make -j4
    ```
    
    If you cannot (or do not want) to use `libgsl-dev` from a package repository, call `configure` with `--enable-download-gsl`:
    
    ```terminal
    ./configure --enable-download-gsl
    ```
    
    If you do not have Internet access, get the tarball manually, copy it to the same directory as `configure` and run again.

 5. Run test suite (optional)
 
    ```terminal
    make check
    ```

 6. Install the binary system wide (optional)
 
    ```terminal
    sudo make install
    ```
    
