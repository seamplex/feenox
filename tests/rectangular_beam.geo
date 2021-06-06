SetFactory("OpenCASCADE");
b = 0.05;
h = 0.1;
l = 1;
Box(1) = {0,0,0, l, b, h};

Transfinite Curve {2, 4, 6, 8} = 2+1;
Transfinite Curve {3, 1, 7, 5} = 4+1;
Transfinite Curve {11, 12, 9, 10} = 20+1;

Transfinite Surface "*";
Transfinite Volume "*";

Physical Surface("volume") = {1};
Physical Volume("left") = {1};

Mesh.RecombineAll = 1;
Mesh.ElementOrder = 2;
Mesh.SecondOrderIncomplete = 1;
