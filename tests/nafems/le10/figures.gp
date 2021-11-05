set grid
set terminal svg size 1200,900 dynamic enhanced mouse standalone


set key above
set grid

# -----------------------------------
cx = 1
set xlabel "Mesh refinement factor"

cy =  3
set ylabel "Stress at D [MPa]"
set output "sigmay-c.svg"
load "plot-sigmay.gp"


cy =  4
set logscale y
set ylabel "Wall time [sec]"
set output "wall-c.svg"
load "plot.gp"


cy =  5
set ylabel "Kernel-space CPU time [sec]"
set output "kernel-c.svg"
replot

cy =  6
set ylabel "User-space CPU time [sec]"
set output "user-c.svg"
replot


cy =  7
set ylabel "Memory [kB]"
set output "memory-c.svg"
replot


# -----------------------------------
cx = 2
set xlabel "Total degrees of freedom"

cy =  3
set logscale x
unset logscale y
set ylabel "Stress at D [MPa]"
set output "sigmay-dofs.svg"
load "plot-sigmay.gp"

cy =  4
set logscale x
set logscale y
set ylabel "Wall time [sec]"
set output "wall-dofs.svg"
load "plot.gp"

cy =  5
set logscale x
set logscale y
set ylabel "Kernel-space CPU time [sec]"
set output "kernel-dofs.svg"
replot


cy =  6
set logscale x
set logscale y
set ylabel "User-space CPU time [sec]"
set output "user-dofs.svg"
replot


cy =  7
set logscale x
set logscale y
set ylabel "Memory [kB]"
set output "memory-dofs.svg"
replot
