SetFactory("OpenCASCADE");
Cylinder(1) = {0, 0, 0, 0, 0, 1, 3.937, Pi/12};

Physical Volume("liquid") = {1};
Physical Surface("front") = {3};
Physical Surface("adiabatic") = {1, 2, 4, 5};

Mesh.MeshSizeMax = 0.1;
Mesh.ElementOrder = 2;

