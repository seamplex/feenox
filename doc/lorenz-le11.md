For example, we can solve the [NAFEMS\ LE11](https://www.nafems.org/publications/resource_center/p18/) “Solid cylinder/Taper/Sphere-Temperature” benchmark like

```{.feenox include="nafems-le11.fee"}
```

Another example would be the famous chaotic [Lorenz’ dynamical system](http://en.wikipedia.org/wiki/Lorenz_system)---the one of the butterfly---whose differential equations are

```{=plain}
dx/dt = σ (y-x)  
dy/dt = x (r-z) - y  
dz/dt = x y - b z  
```

::: {.not-in-format .texi }
```{=latex}
\begin{equation*}
\begin{cases}
\dot{x} &= \sigma \cdot (y - x)  \\
\dot{y} &= x \cdot (r - z) - y   \\
\dot{z} &= x y - b z       \\
\end{cases}
\end{equation*}
```
:::

::: {.not-in-format .plain .latex }
$$\dot{x} = \sigma \cdot (y - x)$$  
$$\dot{y} = x \cdot (r - z) - y$$  
$$\dot{z} = x \cdot y - b \cdot z$$  
:::

where $\sigma=10$, $b=8/3$ and $r=28$ are the classical parameters that generate the butterfly as presented by Edward Lorenz back in his seminal 1963 paper [Deterministic non-periodic flow](http://journals.ametsoc.org/doi/abs/10.1175/1520-0469%281963%29020%3C0130%3ADNF%3E2.0.CO%3B2). We can solve it with FeenoX by writing the equations in the input file as naturally as possible, as illustrated in the input file that follows:

```{.feenox include="lorenz.fee"}
```


Please note the following two points about both cases above:

 1. The input files are very similar to the statements of each problem in plain English words (_rule of clarity_). Take some time to read the [problem statement of the NAFEMS\ LE11 benchmark](doc/design/nafems-le11/nafems-le11.png) and the FeenoX input to see how well the latter matches the former. Same for the Lorenz’ chaotic system. Those with some experience may want to compare them to the inputs decks (sic) needed for other common FEA programs.
 2. By design, 100% of FeenoX’ output is controlled by the user. Had there not been any `PRINT` or `WRITE_MESH` instructions, the output would have been empty, following the _rule of silence_. This is a significant change with respect to traditional engineering codes that date back from times when one CPU hour was worth dozens (or even hundreds) of engineering hours. At that time, cognizant engineers had to dig into thousands of lines of data to search for a single individual result. Nowadays, following the _rule of economy_, it is actually far easier to ask the code to write only what is needed in the particular format that suits the user.
