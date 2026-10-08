set terminal svg size 2000,1000

set xlabel "x [m]"
set ylabel "y [m]"
set size ratio -1
set xrange [0:15]

set output "tennis-top.svg"
plot "tennis-top-0.5.dat" u 2:3 w l title "50% top-spin", \
     "tennis-base.dat"    u 2:3 w l title "100% top-spin", \
     "tennis-top+0.5.dat" u 2:3 w l title "150% top-spin"

set output "tennis-rho.svg"
plot "tennis-rho-0.2.dat" u 2:3 w l title "20% thinner air", \
     "tennis-base.dat"    u 2:3 w l title "Nominal air density", \
     "tennis-rho+0.2.dat" u 2:3 w l title "20% denser air"
