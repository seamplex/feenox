---
title: FeenoX
...

# What is FeenoX?

 a. a finite-element tool with a particular design basis
 b. a syntactically-sweetened way of asking the computer to solve a maths problem
 
 
# Download

## Binaries

## Source tarball

## Git repository

 1. Install mandatory dependences

```
sudo apt-get install git gcc make automake autoconf libgsl-dev
```

 2. Install optional dependences (of course these are optional but recommended)
 
```
sudo apt-get install lib-sundials-dev petsc-dev slepc-dev libreadline-dev
```

 3. Clone Github repository
 
```
git clone https://github.com/seamplex/feenox
```

 4. Boostrap, configure, compile & make
 
```
cd feenox
./autogen.sh
./configure
make
```

 5. Run test suite (optional, this takes a lot of time)
 
``` 
make check
```

 6. Install the binary system wide (optional)
 
```
sudo make install
```

