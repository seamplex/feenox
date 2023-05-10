#!/usr/bin/gnuplot
unset border
unset grid
unset key
unset label
unset xtics
unset ytics
set terminal svg
set output "double-guy-form.svg"
plot "double-guy-form.tsv" u 8:9 w l lt type

