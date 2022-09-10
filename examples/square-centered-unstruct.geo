SetFactory("OpenCASCADE");
Rectangle(1) = {-1, -1, 0, 2, 2, 0};

Mesh.Algorithm = 2;
Mesh.MeshSizeMin = 0.35;
Mesh.MeshSizeMax = 0.5;
Mesh.ElementOrder = 1;

Physical Curve("left") = {4};
Physical Curve("right") = {2};
Physical Curve("bottom") = {1};
Physical Curve("top") = {3};
Physical Surface("bulk") = {1};
