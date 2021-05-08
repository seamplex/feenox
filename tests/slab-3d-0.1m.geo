SetFactory("OpenCASCADE");
l = 0.1;
n = 20;
Box(1) = {0, 0, 0, l, l/n, l/n};

Physical Surface("left") = {1};
Physical Surface("right") = {2};
Physical Volume("bulk") = {1};

Mesh.RecombineAll = 1;

Transfinite Curve {1, 2, 3, 4, 5, 6, 7, 8} = 2;
Transfinite Curve {9, 10, 11, 12} = n+1;
Transfinite Surface "*";
Transfinite Volume "*";
