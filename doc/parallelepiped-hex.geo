SetFactory("OpenCASCADE");

l = 20;
h = 10;

Box(1) = {0, -h/2, -h/2, l, h/2, h/2};
Box(2) = {0, 0,    -h/2, l, h/2, h/2};
Box(3) = {0, 0,    0   , l, h/2, h/2};
Box(4) = {0, -h/2, 0,    l, h/2, h/2};
Coherence;

Physical Point("O") = {3};
Physical Point("A") = {7};
Physical Point("B") = {9};
Physical Point("C") = {13};

Physical Surface("left") = {7, 12, 17, 1};
Physical Surface("right") = {13, 8, 2, 6, 18};
Physical Surface("front") = {19, 3};
Physical Surface("back") = {9, 15};
Physical Surface("top") = {16, 20};
Physical Surface("bottom", 34) = {5, 10};
Physical Volume("bulk", 35) = {4, 1, 3, 2};

/*
Transfinite Curve{1,6} = 8;
Transfinite Curve{2,3,4,5} = 4;
Transfinite Surface{1};
*/

Mesh.ElementOrder = 2;
Mesh.SecondOrderIncomplete = 0;
