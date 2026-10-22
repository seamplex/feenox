#!/usr/bin/gnuplot -persist

set xlabel "x" 
set ylabel "y" 
unset colorbox
set ticslevel 0
unset key
set size ratio 2.0/3.0

set terminal svg
set output "f.svg"
set zlabel "f" 
splot "hyperbolic-paraboloid.dat" pt 52 ps 2 palette,  "2d-interpolation.dat" u 1:2:3 w l palette

set output "g.svg"
set zlabel "g" 
splot "hyperbolic-paraboloid.dat" pt 52 ps 2 palette,  "2d-interpolation.dat" u 1:2:4 w l palette

set output "h.svg"
set zlabel "h" 
splot "hyperbolic-paraboloid.dat" pt 52 ps 2 palette,  "2d-interpolation.dat" u 1:2:5 w l palette



set terminal png
set xrange [-1:3]
set yrange [-1:2]

set output "f.png"
plot "2d-interpolation.dat" u 1:2:3 w image

set output "g.png"
plot "2d-interpolation.dat" u 1:2:4 w image

set output "h.png"
plot "2d-interpolation.dat" u 1:2:5 w image
