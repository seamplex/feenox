The [`tests`](https://github.com/seamplex/feenox/tree/main/tests) directory in the repository has hundreds of
 
 - `grep`-able examples, e.g. to see how to use the keywork [`INTEGRATE`](https://seamplex.com/feenox/doc/feenox-manual.html#integrate), do
 
  ```terminal
   $ cd tests
   $ grep INTEGRATE *.fee
   integrate2d.fee:INTEGRATE f RESULT I1
   integrate2d.fee:INTEGRATE f(x,y)^2 RESULT I2
   write_mesh2d.fee:INTEGRATE f RESULT I
   $ 
   ```

 - [unit and regression tests](https://seamplex.com/feenox/doc/compilation.html#sec:test-suite), 
 - (crude) mathematical verification tests,
 - subdirectories with further case studies
    - [`mms`](https://github.com/seamplex/feenox/tree/main/tests/mms)
      - [`thermal`](https://github.com/seamplex/feenox/tree/main/tests/mms/thermal)
        - [`2d`](https://github.com/seamplex/feenox/tree/main/tests/mms/thermal/2d)
        - [`3d`](https://github.com/seamplex/feenox/tree/main/tests/mms/thermal/3d)
    - [`nafems`](https://github.com/seamplex/feenox/tree/main/tests/nafems)
      - [`le10`](https://github.com/seamplex/feenox/tree/main/tests/nafems/le10)
