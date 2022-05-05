SetFactory("OpenCASCADE");
Merge "spinning-disk-dimensions.geo";
Merge "spinning-disk-unstruct.geo";

Cylinder(1) = {0, 0, 0,  0, 0, t/2,  R, 2*Pi/4};

Physical Surface("symmetry1") = {3};
Physical Surface("symmetry2") = {4};
Physical Surface("symmetry3") = {5};
Physical Volume("bulk") = {1};
