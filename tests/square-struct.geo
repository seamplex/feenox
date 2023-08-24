SetFactory("OpenCASCADE");
Rectangle(1) = {0, 0, 0, 1, 1, 0};

Transfinite Curve "*" = 1+8;
Transfinite Surface "*";
Mesh.RecombineAll = 1;

Physical Curve("left") = {4};
Physical Curve("right") = {2};
Physical Curve("bottom") = {1};
Physical Curve("top") = {3};
Physical Surface("bulk") = {1};
