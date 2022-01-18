SetFactory("OpenCASCADE");

a = 10;
Box(1) = {-a,-a/2,-a/2,a,a,a};
Box(2) = {0,-a/2,-a/2,a,a,a};
Coherence;


Mesh.MeshSizeMax = 3;
Mesh.MeshSizeMin = 3;
Mesh.Algorithm = 1;
Mesh.Optimize = 1;
Mesh.OptimizeNetgen = 1;
Mesh.ElementOrder = 2;
Mesh.SecondOrderLinear = 1;

Physical Surface("left", 1) = {1};
Physical Surface("right", 2) = {7};

Physical Volume("hard", 3) = {1};
Physical Volume("soft", 4) = {2};
