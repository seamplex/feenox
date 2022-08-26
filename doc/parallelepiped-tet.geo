SetFactory("OpenCASCADE");

l = 20;
h = 10;

Box(1) = {0, -h/2, -h/2, l, h, h};
A = newp;
Point(A) = (l, 0, 0);

BooleanFragments{ Volume{1}; Delete; }{ Point{A}; Delete; }

Physical Point("A") = {A};

Physical Surface("left") = {1};
Physical Surface("right") = {2};
Physical Surface("front") = {3};
Physical Surface("back") = {4};
Physical Surface("bottom") = {5};
Physical Surface("top") = {6};

Physical Volume("bulk") = {1};

Mesh.ElementOrder = 1;
Mesh.MeshSizeMin = h/5;
Mesh.MeshSizeMax = h/5;
Mesh.HighOrderOptimize = 1;
