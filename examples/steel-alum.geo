SetFactory("OpenCASCADE");
Box(1) = {0, 0, 0, 5.5, 1, 1};
Box(2) = {5, 1, 0, 5, 1, 1};
Box(3) = {10, 1, 0, 5, 1, 1};
Coherence;
Mesh.MeshSizeMax = 0.2;
Mesh.ElementOrder = 2;

Physical Volume("pink") = { 1 };
Physical Volume("blue") = { 2, 3 };
Physical Surface("fixed", 35) = {1};
Physical Surface("2p", 36) = {8};
Physical Surface("p", 37) = {14};
Physical Curve("pin", 38) = {19};
