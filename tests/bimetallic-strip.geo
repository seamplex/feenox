SetFactory("OpenCASCADE");
l = 10;
w = 1;
t = 0.05;
Box(1) = {0, -w/2, 0, l, w, +t};
Box(2) = {0, -w/2, 0, l, w, -t};
Coherence;

Mesh.RecombineAll = 1;
Mesh.ElementOrder = 2;
Mesh.SecondOrderIncomplete = 1;

Physical Point("A") = {1};
Physical Point("B") = {4};
Physical Surface("left") = {1, 7};
Physical Volume("top") = {1};
Physical Volume("bottom") = {2};

Transfinite Curve {19, 9, 10, 20, 11, 12} = 1+36;
Transfinite Curve {15, 4, 2, 18, 8, 6} = 1+4;
Transfinite Curve {14, 3, 13, 1, 17, 7, 16, 5} = 1+4;
Transfinite Surface "*";
Transfinite Volume "*";
