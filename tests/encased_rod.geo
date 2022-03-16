SetFactory("OpenCASCADE");
Box(1) = {0,0,0,    0.5, 0.01, 0.04};
Box(2) = {0,0.01,0, 0.5, 0.02, 0.04};
Box(3) = {0,0.03,0, 0.5, 0.01, 0.04};
Coherence;

Physical Volume("ss", 1) = {1, 3};
Physical Volume("cu", 2) = {2};
Physical Surface("left", 3) = {12, 7, 1};
Physical Surface("right", 4) = {13, 8, 2};

Mesh.MeshSizeMax = 0.005;
Mesh.Optimize = 1;
Mesh.OptimizeNetgen = 1;
Mesh.Algorithm = 8;
