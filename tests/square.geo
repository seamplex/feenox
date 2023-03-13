SetFactory("OpenCASCADE");
Rectangle(1) = {0, 0, 0, 1, 1, 0};

Mesh.MeshSizeMax = 0.5;
Mesh.MeshSizeMin = 0.5;

Physical Curve("left") = {4};
Physical Curve("right") = {2};
Physical Curve("bottom") = {1};
Physical Curve("top") = {3};
Physical Surface("bulk") = {1};
