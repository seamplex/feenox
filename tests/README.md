# Simple unitary tests

This directory contains FeenoX input files that tests one tiny part of the whole functionality, i.e. _unit tests_.
The main objective is to detect regressions which might be introduced into the code that break existing functionality.

These input files can also be used as a reference and/or examples of how to use a certain keyworkd. Just `grep` for the keyword over `*.fee` to see an example of usage:

```
$ grep INTEGRATE *.fee
integrate2d.fee:INTEGRATE f RESULT I1
integrate2d.fee:INTEGRATE f(x,y)^2 RESULT I2
write_mesh2d.fee:INTEGRATE f RESULT I
$ 
```

See directory [../examples] for more complex examples, which can also be seen as _integration tests_.


