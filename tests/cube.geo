SetFactory("OpenCASCADE");

a = 1;
Box(1) = {0,0,0,a,a,a};

Mesh.MeshSizeMax = a/4;
Mesh.MeshSizeMin = a/4;
Mesh.Algorithm = 1;
Mesh.Optimize = 1;
// Mesh.OptimizeNetgen = 1;
Mesh.ElementOrder = 2;
Mesh.SecondOrderLinear = 1;

Physical Surface("left", 1) = {1};
Physical Surface("right", 2) = {2};
Physical Surface("front", 3) = {3};
Physical Surface("back", 4) = {4};
Physical Surface("bottom", 5) = {5};
Physical Surface("top", 6) = {6};
Physical Volume("bulk", 7) = {1};
