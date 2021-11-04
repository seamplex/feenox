set grid
set terminal svg size 1200,900 dynamic enhanced mouse standalone

set key above

set xlabel "Total degrees of freedom"

# TODO: vs. col 1

c = 3
set ylabel "Stress at D [Mpa]"
set output "sigmay-xxx-linear.svg"
load "plot.gp"

c = 4
set ylabel "Wall time [sec]"
set output "wall-xxx-linear.svg"
replot

c = 5
set ylabel "Kernel-space CPU time [sec]"
set output "kernel-xxx-linear.svg"
replot

c = 6
set ylabel "User-space CPU time [sec]"
set output "user-xxx-linear.svg"
replot

c = 7
set ylabel "Memory [kB]"
set output "user-xxx-linear.svg"
replot
