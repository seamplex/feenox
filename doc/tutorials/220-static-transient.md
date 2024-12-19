---
title: Static and transient cases
subtitle: FeenoX Tutorial #2
...


Whenever the special variable end_time is non-zero, wasora enters into transient mode. These examples introduce transient problems by illustrating how first-order lags can be used to filter signals.


This example generates a signal $r(t)$ which is zero except for $a<t<b$, where it takes the value one. Then, the signal $y(t)$ is computed as a first-order lag of $r(t)$ with a characteristic time\ $\tau$. τ\tauτ. The output consists of three columns containing ttt, r(t)r(t)r(t) and y(t)y(t)y(t). By using the keyword HEADER a commented line is pre-prended to the output with a textual representation of the expressions passed to PRINT which are automatically understood by qdp so a reasonable figure can be obtained with virtually no effort, as shown in terminal mimic where the output of wasora is piped to qdp.

# this is a transient problem and lasts 5 units of time
end_time = 5
# each time step is equal to 1/20th of a unit of time
dt = 1/20

# some parameters, which we define as constants
CONST a b tau
a = 1
b = 3
tau = 1.234

# signal r is equal to zero except for a < t < b
r = 0
r[a:b] = 1

# signal y is equal to signal r filtered through a lag
# of characteristic time tau
y = lag(r, tau)

PRINT t r y

# exercise: investigate how the result of the lag
# depends on the time step

$ wasora lag.was | qdp -o lag
$ 

Output of lag.was
1.2 compact.was

Instead of writing the long input shown in lag.was, we could have obtained the same result with a couple of lines. Indeed, the terminal shows that the output of this input is the same as the one of the previous longer example.

# the preceding example could have been written in fewer
# lines as follows (although the SPOT rule is broken)
end_time = 5
dt = 1/20
PRINT t heaviside(t-1)-heaviside(t-3) lag(heaviside(t-1)-heaviside(t-3),1.234)

$ wasora lag.was > lag.dat
$ wasora compact.was > compact.dat
$ diff -s lag.dat compact.dat
Files lag.dat and compact.dat are identical
$ 

The reported difference is due to the presence of the HEADER keyword in the first input so qdp can automatically label the bullets. kate
