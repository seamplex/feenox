SetFactory("OpenCASCADE");
Rectangle(1) = {-1, -1, 0, 2, 2, 0};

Transfinite Curve "*" = 32+1;
Transfinite Surface "*";
Mesh.RecombineAll = 1;

Physical Curve("left") = {4};
Physical Curve("right") = {2};
Physical Curve("bottom") = {1};
Physical Curve("up") = {3};
Physical Surface("bulk") = {1};
