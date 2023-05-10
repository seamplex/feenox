---
title: Ordinary Differential Equations & Differential-Algebraic Equations
titleblock: |
 Ordinary Differential Equations & Differential-Algebraic Equations
 ==================================================================
lang: en-US
toc: true 
...
# Lorenz' attractor---the one with the butterfly

Solve

$$
\begin{cases}
\dot{x} &= \sigma \cdot (y - x)  \\
\dot{y} &= x \cdot (r - z) - y   \\
\dot{z} &= x y - b z             \\
\end{cases}
$$

for $0 < t < 40$ with initial conditions

$$
\begin{cases}
x(0) = -11  \\
y(0) = -16  \\
z(0) = 22.5 \\
\end{cases}
$$

and $\sigma=10$, $r=28$ and $b=8/3$, which are the classical parameters
that generate the butterfly as presented by Edward Lorenz back in his
seminal 1963 paper [Deterministic non-periodic
flow](http://journals.ametsoc.org/doi/abs/10.1175/1520-0469%281963%29020%3C0130%3ADNF%3E2.0.CO%3B2).
This example's input file ressembles the parameters, inital conditions
and differential equations of the problem as naturally as possible with
an ASCII file.


```feenox
PHASE_SPACE x y z     # Lorenz attractor’s phase space is x-y-z
end_time = 40         # we go from t=0 to 40 non-dimensional units

sigma = 10            # the original parameters from the 1963 paper
r = 28
b = 8/3

x_0 = -11             # initial conditions
y_0 = -16
z_0 = 22.5

# the dynamical system's equations written as naturally as possible
x_dot = sigma*(y - x)
y_dot = x*(r - z) - y
z_dot = x*y - b*z

PRINT t x y z        # four-column plain-ASCII output
```


```terminal
$ feenox lorenz.fee > lorenz.dat
$ gnuplot lorenz.gp
$ python3 lorenz.py
$ sh lorenz2x3d.sh < lorenz.dat > lorenz.html

```


::: {#fig:ex_lorenz}
![Gnuplot](lorenz-gnuplot.svg){width=48%}
![Matplotlib](lorenz-matplotlib.png){width=48%}

The Lorenz attractor computed with FeenoX plotted with two different tools
:::


# The double pendulum

Find the location of the two bobs vs time in the double pendulum
in @fig:double-pendulum.

![A chaotic double pendulum](double.svg){#fig:double-pendulum
width="60%"}

Use these four different approaches:

a.  Hamiltonian formulation with numerical derivatives

    $$
    \begin{aligned}
    \mathcal{H}(\theta_1, \theta_2, p_1, p_2) =&
     - \frac{\ell_2^2 m_2 p_1^2 - 2 \ell_1 \ell_2 m_2 \cos(\theta_1-\theta_2) p_1 p_2 + \ell_1^2 (m_1+m_2) p_2^2}
     {\ell_1^2 \ell_2^2 m_2 \left[-2m_1-m_2+m_2\cos\Big(2(\theta_1-\theta_2)\Big)\right]} \\
     & - \Big[ m_1 g \ell_1 \cos \theta_1 + m_2 g (\ell_1 \cos \theta_1 + \ell_2 \cos \theta_2) \Big]
    \end{aligned}
    $$

    $$
    \begin{cases}
    \displaystyle  \dot{\theta}_1 &= \displaystyle +\frac{\partial \mathcal{H}}{\partial p_1} \\
    \displaystyle  \dot{\theta}_2 &= \displaystyle +\frac{\partial \mathcal{H}}{\partial p_2} \\
    \displaystyle  \dot{p}_1 &= \displaystyle -\frac{\partial \mathcal{H}}{\partial \theta_1} \\
    \displaystyle  \dot{p}_2 &= \displaystyle -\frac{\partial \mathcal{H}}{\partial \theta_2} \\
    \end{cases}
    $$

    ``` feenox
    # the double pendulum solved by the hamiltonian formulation
    # and numerically computing its derivatives

    PHASE_SPACE theta1 theta2 p1 p2
    VAR theta1' theta2' p1' p2'

    H(theta1,theta2,p1,p2) = \
              - (m1*g*l1*cos(theta1) + m2*g*(l1*cos(theta1) \
              + l2*cos(theta2))) \
              - (l2^2*m2*p1^2 - 2*l1*l2*m2*cos(theta1-theta2)*p1*p2 + \
                   l1^2*(m1+m2)*p2^2)/(l1^2*l2^2*m2 \ 
                     * (-2*m1-m2+m2*cos(2*(theta1-theta2))))


    theta1_dot .= +derivative(H(theta1,theta2,p1',p2), p1', p1)
    theta2_dot .= +derivative(H(theta1,theta2,p1,p2'), p2', p2)

    p1_dot     .= -derivative(H(theta1',theta2,p1,p2), theta1', theta1)
    p2_dot     .= -derivative(H(theta1,theta2',p1,p2), theta2', theta2)
    ```

b.  Hamiltonian formulation with analytical derivatives

    $$
    \begin{cases}
    \dot{\theta}_1 &= \displaystyle         \frac{p_1 \ell_2 - p_2 \ell_1 \cos(\theta_1-\theta_2)}{\ell_1^2 \ell_2 [m_1 + m_2 \sin^2(\theta_1-\theta_2)]} \\
    \dot{\theta}_2 &= \displaystyle  \frac{p_2 (m_1+m_2)/m_2 \ell_1 - p_1 \ell_2 \cos(\theta_1-\theta_2)}{\ell_1 \ell_2^2 [m_1 + m_2 \sin^2(\theta_1-\theta_2)]} \\
    \dot{p_1} &= \displaystyle -(m_1+m_2) g \ell_1 \sin(\theta_1) - c_1 + c_2 \\
    \dot{p_2} &= \displaystyle -m_2 g \ell_2 \sin(\theta_2) + c_1 - c_2
    \end{cases}
    $$ where the expressions $c_1$ and $c_2$ are

    $$\begin{aligned}
    c1 &= \frac{p_1 p_2 \sin(\theta_1-\theta_2)}{\ell_1 \ell_2 \Big[m_1+m_2 \sin(\theta_1-\theta_2)^2\Big]} \\
    c2 &= \frac{\Big[ p_1^2 m_2 \ell_2^2 - 2 p_1 p_2 m_2 \ell_1 \ell_2 \cos(\theta_1-\theta_2)
       + p_2^2 (m_1+m_2) \ell_1^2)\Big] \sin(2 (\theta_1-\theta_2)}{
      2 \ell_1^2 \ell_2^2 \left[m_1+m_2 \sin^2(\theta_1-\theta_2)\right]^2}
    \end{aligned}
    $$

    ``` feenox
    # the double pendulum solved by the hamiltonian formulation
    # and analytically computing its derivatives

    PHASE_SPACE theta1 theta2 p1 p2 c1 c2

    theta1_dot .=            (p1*l2 - p2*l1*cos(theta1-theta2))/(l1^2*l2*(m1 + m2*sin(theta1-theta2)^2))
    theta2_dot .= (p2*(m1+m2)/m2*l1 - p1*l2*cos(theta1-theta2))/(l1*l2^2*(m1 + m2*sin(theta1-theta2)^2))

    p1_dot .= -(m1+m2)*g*l1*sin(theta1) - c1 + c2
    p2_dot .=      -m2*g*l2*sin(theta2) + c1 - c2

    c1 .= p1*p2*sin(theta1-theta2)/(l1*l2*(m1+m2*sin(theta1-theta2)^2))
    c2 .= { (p1^2*m2*l2^2 - 2*p1*p2*m2*l1*l2*cos(theta1-theta2)
             + p2^2*(m1+m2)*l1^2)*sin(2*(theta1-theta2))/
            (2*l1^2*l2^2*(m1+m2*sin(theta1-theta2)^2)^2) }
    ```

c.  Lagrangian formulation with numerical derivatives

    $$
    \begin{aligned}
    \mathcal{L}(\theta_1, \theta_2, \dot{\theta}_1, \dot{\theta}_2) =& 
    \frac{1}{2} m_1 \ell_1^2 \dot{\theta}_1^2 +
    \frac{1}{2} m_2 \left[\ell_1^2 \dot{\theta}_1^2 + \ell_2^2 \dot{\theta}_2^2 + 2 \ell_1 \ell_2 \dot{\theta}_1 \dot{\theta}_2 \cos(\theta_1-\theta_2)\right] + \\
    & m_1 g  \ell_1\cos \theta_1 + m_2 g \left(\ell_1\cos \theta_1 + \ell_2 \cos \theta_2 \right)
    \end{aligned}
    $$

    $$
    \begin{cases}
    \displaystyle \frac{\partial}{\partial t}\left(\frac{\partial \mathcal{L}}{\partial \dot{\theta}_1}\right) &= \displaystyle \frac{\partial \mathcal{L}}{\partial \theta_1} \\
    \displaystyle  \frac{\partial}{\partial t}\left(\frac{\partial \mathcal{L}}{\partial \dot{\theta}_2}\right) &= \displaystyle \frac{\partial \mathcal{L}}{\partial \theta_2}
    \end{cases}
    $$

    ``` feenox
    # the double pendulum solved by the lagrangian formulation
    # and numerically computing its derivatives

    PHASE_SPACE theta1 theta2 dL_dthetadot1 dL_dthetadot2
    VAR theta1' theta2' theta1_dot' theta2_dot'

    L(theta1,theta2,theta1_dot,theta2_dot) = {
    # kinetic energy of m1
     1/2*m1*l1^2*theta1_dot^2 + 
    # kinetic energy of m2
     1/2*m2*(l1^2*theta1_dot^2 + l2^2*theta2_dot^2 + 2*l1*l2*theta1_dot*theta2_dot*cos(theta1-theta2))
     + ( 
    # potential energy of m1 
     m1*g *  l1*cos(theta1) +
    # potential energy of m2
     m2*g * (l1*cos(theta1) + l2*cos(theta2))
     ) }

    # there is nothing wrong with numerical derivatives, is there?
    dL_dthetadot1 .= derivative(L(theta1, theta2, theta1_dot', theta2_dot), theta1_dot', theta1_dot)
    dL_dthetadot2 .= derivative(L(theta1, theta2, theta1_dot, theta2_dot'), theta2_dot', theta2_dot)

    dL_dthetadot1_dot .= derivative(L(theta1', theta2,theta1_dot, theta2_dot), theta1', theta1)
    dL_dthetadot2_dot .= derivative(L(theta1, theta2',theta1_dot, theta2_dot), theta2', theta2)

    ```

d.  Lagrangian formulation with analytical derivatives

    $$
    \begin{cases}
    0 &= (m_1+m_2) \ell_1^2 \ddot{\theta}_1 + m_2 \ell_1 \ell_2 \ddot{\theta}_2 \cos(\theta_1-\theta_2) + m_2 \ell_1 \ell_2 \dot{\theta}_2^2 \sin(\theta_1-\theta_2) + \ell_1 (m_1+m_2) g \sin(\theta_1) \\
    0 &= m_2 \ell_2^2 \ddot{\theta}_2 + m_2 \ell_1 \ell_2 \ddot{\theta}_1 \cos(\theta_1-\theta_2) - m_2 \ell_1 \ell_2 \dot{\theta}_1^2 \sin(\theta_1-\theta_2) + \ell_2 m_2 g \sin(\theta_2)
    \end{cases}
    $$

    ``` feenox
    # the double pendulum solved by the lagrangian formulation
    # and analytically computing its derivatives

    PHASE_SPACE theta1 theta2 omega1 omega2

    # reduction to a first-order system
    omega1 .= theta1_dot
    omega2 .= theta2_dot

    # lagrange equations
    0 .= (m1+m2)*l1^2*omega1_dot + m2*l1*l2*omega2_dot*cos(theta1-theta2) + \
     m2*l1*l2*omega2^2*sin(theta1-theta2) + l1*(m1+m2)*g*sin(theta1)


    0 .= m2*l2^2*omega2_dot + m2*l1*l2*omega1_dot*cos(theta1-theta2) - \
     m2*l1*l2*omega1^2*sin(theta1-theta2) + l2*m2*g*sin(theta2)
    ```

The combination Hamilton/Lagrange and numerical/analytical is given in
the command line as arguments `$1` and `$2` respectively.


```feenox
# the double pendulum solved using different formulations

# parameters
end_time = 10
m1 = 0.3
m2 = 0.2
l1 = 0.3
l2 = 0.25
g = 9.8

# inital conditions
theta1_0 = pi/2
theta2_0 = pi

# include the selected formulation
DEFAULT_ARGUMENT_VALUE 1 hamilton
DEFAULT_ARGUMENT_VALUE 2 numerical
INCLUDE double-$1-$2.fee

# output the results vs. time
PRINT t theta1 theta2 theta1_dot theta2_dot \
      l1*sin(theta1)                -l1*cos(theta1) \
      l1*sin(theta1)+l2*sin(theta2) -l1*cos(theta1)-l2*cos(theta2) 
```


```terminal
$ for guy in hamilton lagrange; do
   for form in numerical analytical; do
     feenox double.fee $guy $form > double-${guy}-${form}.tsv;
     m4 -Dguy=$guy -Dform=$form -Dtype=$RANDOM double.gp.m4 | gnuplot -;
   done;
  done
$ 

```


::: {#fig:double-results}
![Hamilton numerical](double-hamilton-numerical.svg){width=48%}
![Hamilton analytical](double-hamilton-analytical.svg){width=48%}

![Lagrange numerical](double-lagrange-numerical.svg){width=48%}
![Lagrange analytical](double-lagrange-analytical.svg){width=48%}

Position of the double pendulum's $m_2$ solved with four (slightly) different formulations
:::


