# Simple unitary tests

This directory contains FeenoX input files that tests one tiny part of the whole functionality, i.e. _unit tests_.
The main objective is to detect regressions which might be introduced into the code that break existing functionality, but also work as 

   - examples,
   - unit tests,
   - regression tests, and/or
   - (crude) mathematical verification tests.

 * These tests are run when doing `make check` after [compiling the source code](../doc/compile.md). 

 * The test cases usually return a single number (which should be near zero) comparing the numerical result with the expected one.
 
 * These input files can also be used as a reference and/or examples of how to use a certain keyworkd. Just `grep` for the keyword over `*.fee` to see an example of usage:

   ```terminal
   $ grep INTEGRATE *.fee
   integrate2d.fee:INTEGRATE f RESULT I1
   integrate2d.fee:INTEGRATE f(x,y)^2 RESULT I2
   write_mesh2d.fee:INTEGRATE f RESULT I
   $ 
   ```
 
 
 * Feel free to propose benchmark problems to add to the suite in the [Github discussion page](https://github.com/seamplex/feenox/discussions).

 * See [`mms`](https://github.com/seamplex/feenox/tree/main/tests/mms) for verification cases using the Method of Manufactured Solutions.
