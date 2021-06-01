# run this file through Gnuplot to get an SVG of the Lorenz attractor computed by FeenoX
set terminal svg size 1600,900
set output "lorenz-gnuplot.svg"
set xlabel "x"
set ylabel "y"
set zlabel "z"
set ticslevel 0
splot "lorenz.dat" u 2:3:4 w l lc rgb "#3fc5e4" ti ""
