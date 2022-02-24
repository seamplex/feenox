//
SetFactory("OpenCASCADE");
r = 4e-3;
l = 5e-3;
Cylinder(1) = {0, 0, 0, 0, 0, l, r, Pi/6};
Physical Surface("symmetry") = {2, 3, 4, 5};
Physical Surface("external") = {1};
Physical Volume("bulk") = {1};

Mesh.ElementOrder = 1;
Mesh.OptimizeNetgen = 1;
Mesh.HighOrderOptimize = 1;
Mesh.MeshSizeMin = 0.5*(r+l)/20;
Mesh.MeshSizeMax = 0.5*(r+l)/10;
