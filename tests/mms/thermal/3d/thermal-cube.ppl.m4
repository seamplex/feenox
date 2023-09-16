set preamble "\usepackage{amsmath}"

set width 18*unit(cm)
set size ratio 9.0/16.0

set axis x arrow nomirrored
set axis y arrow nomirrored
set key top right
set keycolumns 3

set grid
set terminal pdf

set logscale x
set logscale y

set xlabel "element size~$h$"
# set xrange [2e-3:2e-1]

set key above

load "thermal-cube`'n`'-fits.ppl"

set ylabel "$e_\infty = \max \Big| T(x,y,z)-T_\text{mms}(x,y,z) \Big|$"
set title "FeenoX---Cube case `'n`'--$e_\infty$"
set output "thermal-cube`'n`'-einf.pdf"
load "thermal-cube`'n`'-einf.ppl"

set ylabel "$e_2 = \sqrt{\int \left[T(x,y,z)-T_\text{mms}(x,y,z)\right]^2 \, dx \, dy \, dz}$"
set title "FeenoX---Cube case `'n`'--$e_2$"
set output "thermal-cube`'n`'-e2.pdf"
load "thermal-cube`'n`'-e2.ppl"

