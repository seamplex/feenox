// --- geometry -------------------------------------------------
SetFactory("OpenCASCADE");
a = 5;
b = 10;
Rectangle(1) = {0, 0, 0, a, a};
Rectangle(2) = {0, 0, 0, b, b};
Coherence;

// --- physical groups ------------------------------------------
Physical Surface("src") = {1};
Physical Surface("abs") = {2};
Physical Curve("mirror") = {3, 8, 7, 6};
Physical Curve("vacuum") = {4, 5};

// --- meshing options ------------------------------------------
n1 = 8;
n2 = 32;
Mesh.MeshSizeMax = a/n1;
Mesh.MeshSizeMin = a/n2;

Mesh.Algorithm = 8;
Mesh.Optimize = 1;
Mesh.OptimizeNetgen = 1;
Mesh.RecombineAll = 1;
Mesh.ElementOrder = 2;

// local refinements
Field[1] = Distance;
Field[1].EdgesList = {1,2};
Field[2] = Threshold;
Field[2].IField = 1;
Field[2].LcMin = Mesh.MeshSizeMin;
Field[2].LcMax = Mesh.MeshSizeMax;
Field[2].DistMin = 2*Mesh.MeshSizeMin;
Field[2].DistMax = 5*Mesh.MeshSizeMin;
Background Field = {2};
