SetFactory("OpenCASCADE");
Merge "spinning-disk-dimensions.geo";
Merge "spinning-disk-unstruct.geo";

a = 0.01;

Cylinder(1) = {0, 0, 0,  0, 0, t/2,  R, 2*Pi/4};
Cylinder(2) = {0, 0, 0,  0, 0, t/2,  a, 2*Pi/4};
BooleanDifference{ Volume{1}; Delete; }{ Volume{2}; Delete; }

Physical Surface("symmetry1") = {11};
Physical Surface("symmetry2") = {9};
Physical Surface("symmetry3") = {10};
Physical Surface("inner") = {6};
Physical Volume("bulk") = {1};
