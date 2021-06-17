 1. Install mandatory dependencies

    ```{.terminal style=terminal}
    sudo apt-get install git gcc make automake autoconf libgsl-dev
    ```

 2. Install optional dependencies (of course these are _optional_ but recommended)
 
    ```{.terminal style=terminal}
    sudo apt-get install lib-sundials-dev petsc-dev slepc-dev libreadline-dev
    ```

 3. Clone Github repository
 
    ```{.terminal style=terminal}
    git clone https://github.com/seamplex/feenox
    ```

 4. Boostrap, configure, compile & make
 
    ```{.terminal style=terminal}
    cd feenox
    ./autogen.sh
    ./configure
    make
    ```

 5. Run test suite (optional, this might take some time)
 
    ```{.terminal style=terminal}
    make check
    ```

 6. Install the binary system wide (optional)
 
    ```{.terminal style=terminal}
    sudo make install
    ```
