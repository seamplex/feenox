SetFactory("OpenCASCADE");
Rectangle(1) = {0, 0, 0, 1, 1, 0};
Rectangle(2) = {1, 0, 0, 1, 1, 0};
Coherence;

Mesh.MeshSizeMax = 0.1;
Mesh.MeshSizeMin = 0.1;
Recombine Surface {2};

Physical Curve("left") = {4};
Physical Curve("right") = {6};
Physical Curve("bottom") = {1, 5};
Physical Curve("top") = {3, 7};
Physical Surface("yellow") = {1};
Physical Surface("cyan") = {2};
