SetFactory("OpenCASCADE");
Box(1) = {0, 0, 0, 1000, 100, 50};
Mesh.MeshSizeMax = 25;
Mesh.ElementOrder = 2;
Physical Surface("bottom", 1) = {1};
Physical Surface("top", 2) = {2};
Physical Volume("volume", 3) = {1};
