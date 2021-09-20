SetFactory("OpenCASCADE");

l = 20;
h = 10;

Box(1) = {0, -h/2, -h/2, l, h, h};
O = newp;
Point(O) = (0, 0, 0);

A = newp;
Point(A) = (l, 0, 0);

B = newp;
Point(B) = (0, h/2, 0);

C = newp;
Point(C) = (0, 0, h/2);

BooleanFragments{ Volume{1}; Delete; }{ Point{A}; Delete; }
BooleanFragments{ Volume{1}; Delete; }{ Point{O}; Delete; }
BooleanFragments{ Volume{1}; Delete; }{ Point{B}; Delete; }
BooleanFragments{ Volume{1}; Delete; }{ Point{C}; Delete; }

Physical Point("O",1) = {O};
Physical Point("A",2) = {A};
Physical Point("B",3) = {17};
Physical Point("C",4) = {12};

Physical Surface("left", 5) = {1};
Physical Surface("right", 6) = {2};

Physical Volume("bulk", 7) = {1};

Mesh.MeshSizeMin = h/5;
Mesh.MeshSizeMax = h/5;
Mesh.ElementOrder = 2;
Mesh.HighOrderOptimize = 1;


