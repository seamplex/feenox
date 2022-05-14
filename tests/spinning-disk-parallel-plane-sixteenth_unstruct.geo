SetFactory("OpenCASCADE");
Merge "spinning-disk-dimensions.geo";
Merge "spinning-disk-unstruct.geo";

Point(1) = {0,0,0};
Point(2) = {R,0,0};
Point(3) = {R*Cos(2*Pi/16),R*Sin(2*Pi/16),0};

Circle(1) = {2, 1, 3};
Line(2) = {3, 1};
Line(3) = {1, 2};
Curve Loop(1) = {1, 2, 3};
Plane Surface(1) = {1};

Physical Surface("bulk") = {1};
Physical Curve("symmetry1") = {3};
Physical Curve("symmetry2") = {2};
