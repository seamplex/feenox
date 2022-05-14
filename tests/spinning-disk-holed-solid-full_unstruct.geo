SetFactory("OpenCASCADE");
Merge "spinning-disk-dimensions.geo";
Merge "spinning-disk-unstruct.geo";

a = 0.01;

Cylinder(1) = {0, 0, -t/2,  0, 0, t,  R};
Cylinder(2) = {0, 0, -t/2,  0, 0, t,  a};
BooleanDifference{ Volume{1}; Delete; }{ Volume{2}; Delete; }

Physical Surface("inner") = {4};
Physical Volume("bulk") = {1};
