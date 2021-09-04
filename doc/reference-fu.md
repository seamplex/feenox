#  `derivative`

Computes the derivative of the expression $f(x)$ 
given in the first argument with respect to the variable $x$
given in the second argument at the point $x=a$ given in
the third argument using an adaptive scheme.
The fourth optional argument $h$ is the initial width
of the range the adaptive derivation method starts with. 
The fifth optional argument $p$ is a flag that indicates
whether a backward ($p < 0$), centered ($p = 0$) or forward ($p > 0$)
stencil is to be used.
This functional calls the GSL functions
`gsl_deriv_backward`, `gsl_deriv_central` or `gsl_deriv_forward`
according to the indicated flag $p$.
Defaults are $h = (1/2)^{-10} \approx 9.8 \times 10^{-4}$ and $p = 0$.


~~~feenox
derivative(f(x), x, a, [h], [p])  
~~~

::: {.not-in-format .man}
$$\left. \frac{d}{dx} \Big[ f(x) \Big] \right|_{x = a} $$
:::



