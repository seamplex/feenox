PROBLEM thermal 3d
READ_MESH fins.msh

include(forloop.m4)
BC convection h=10 Tref=-5 forloop(i, 1, 5, `PHYSICAL_GROUP "surf_1_`'i"' ) forloop(i, 7, 26, `PHYSICAL_GROUP "surf_1_`'i"' )

BC surf_1_6 q=1000
k = 237
SOLVE_PROBLEM
WRITE_MESH fins.vtk T
