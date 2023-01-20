//
SetFactory("OpenCASCADE");
Box(1) = {0, 0, 0, 1, 1, 4};
Box(2) = {0, 0, 4, 1, 1, 3};
Box(3) = {0, 0, 7, 1, 1, 3};
Coherence;

Physical Surface("bottom") = {5};
Physical Surface("top") = {16};
Physical Surface("one") = {11};
Physical Surface("two") = {6};
Physical Volume("bulk") = {1,2,3};

Mesh.MeshSizeMax = 0.5;
// Mesh.OptimizeNetgen = 1;
Mesh.ElementOrder = 2;
