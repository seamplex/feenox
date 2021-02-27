SetFactory("OpenCASCADE");
Cylinder(1) = {0, 0, 0, 1, 0, 0, 1, 2*Pi};

Physical Surface("bottom",1) = {3};
Physical Surface("top",2) = {2};
Physical Volume("bulk",3) = {1};

Mesh.MeshSizeMax = 2;
Mesh.MeshSizeMin = 2;

Mesh.ElementOrder = 2;
