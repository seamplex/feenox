---
title: Neutron transport using $S_N$
titleblock: |
 Neutron transport using $S_N$
 =============================
lang: en-US
toc: true 
...
# Reed's problem

Reed's problem ([William H. Reed (1971) New Difference Schemes for the
Neutron Transport Equation, Nuclear Science and Engineering, 46:2,
309-314, DOI: 10.13182/NSE46-309](http://dx.doi.org/10.13182/NSE46-309))
is a common test problem for transport codes. It is comprised of
heterogeneous materials with strong absorber, vacuum, and scattering
regions. These regions are valuable to testing different aspects of
numerical discretizations.

![Geometry of the 1D Reed's problem
(1971)](reed-problem.svg){width="100%"}


```feenox
PROBLEM neutron_sn DIM 1 GROUPS 1 SN $1
READ_MESH reed.msh
 
MATERIAL source1       S1=50 Sigma_t1=50 Sigma_s1.1=0
MATERIAL absorber      S1=0  Sigma_t1=5  Sigma_s1.1=0
MATERIAL void          S1=0  Sigma_t1=0  Sigma_s1.1=0
MATERIAL source2       S1=1  Sigma_t1=1  Sigma_s1.1=0.9
MATERIAL reflector     S1=0  Sigma_t1=1  Sigma_s1.1=0.9

BC left  mirror
BC right vacuum

SOLVE_PROBLEM

PRINT_FUNCTION phi1
```


```terminal
$ gmsh -1 reed.geo
$ [...]
$ for n in 2 4 6 8; do feenox reed.fee ${n} | sort -g > reed-s${n}.csv; done
$

```


The solutions obtained in FeenoX with $S_2$, $S_4$, $S_6$ and $S_8$ are plotted and compared against and independent solution from <https://www.drryanmc.com/solutions-to-reeds-problem/>.

![Solution of the Reed’s problem](reed-flux.svg){width=100%}


