//
SetFactory("OpenCASCADE");
l = 15;
w = 5;
t = 1;
r = 0.5;

Box(1) = {-l/2, -w/2, -t/2, l, w, t};
Cylinder(2) = {0, 0, -t, 0, 0, 2*t, r};
BooleanDifference{ Volume{1}; Delete; }{ Volume{2}; Delete; }

Physical Surface("left") = {1};
Physical Surface("right") = {6};
Physical Surface("hole") = {7};
Physical Volume("bulk") = {1};

Mesh.MeshSizeMax = t;
Mesh.MeshSizeMin = 0.1*t;
Mesh.MeshSizeFromCurvature = 24;
// Mesh.OptimizeNetgen = 1;
Mesh.ElementOrder = 2;
