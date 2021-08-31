SetFactory("OpenCASCADE");

Rectangle(1) = {0, 0, 0, 0.6, 1.0, 0};

Physical Curve("AB") = {1};
Physical Curve("BC") = {2};
Physical Curve("CD") = {3};
Physical Curve("DA") = {4};
Physical Surface("bulk") = {1};

Mesh.RecombineAll = 1;
Mesh.ElementOrder = 1;

k = 50;
Transfinite Curve {1, 3} = k*5+1;
Transfinite Curve {4, 2} = k*3+1;
Transfinite Surface "*";
