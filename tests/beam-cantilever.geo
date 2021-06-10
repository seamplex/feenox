SetFactory("OpenCASCADE");

l = 10;
b = 0.5;
h = 1;

Box(1) = {0,-b/2,-h/2, l, b, h};

Physical Surface("left", 1) = {1};
Physical Volume("bulk", 2) = {1};

Mesh.MeshSizeMin = 2*b;
Mesh.MeshSizeMax = 2*b;

Mesh.ElementOrder = 1;
Mesh.Optimize = 1;
Mesh.OptimizeNetgen = 1;
Mesh.HighOrderOptimize = 2;
