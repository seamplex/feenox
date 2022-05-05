SetFactory("OpenCASCADE");
Merge "spinning-disk-dimensions.geo";
Merge "spinning-disk-unstruct.geo";

Cylinder(1) = {0, 0, 0,  0, 0, t/2,  R};

Physical Surface("symmetry") = {3};
Physical Volume("bulk") = {1};
