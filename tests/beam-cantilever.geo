// https://autofem.com/examples/determining_natural_frequencie.html
SetFactory("OpenCASCADE");

L = 0.5;
b = 0.05;
h = 0.02;

Box(1) = {0,-b/2,-h/2, L, b, h};

Physical Surface("left", 1) = {1};
Physical Surface("right", 2) = {2};
Physical Volume("bulk", 3) = {1};

Transfinite Curve {1, 3, 5, 7} = 2/Mesh.ElementOrder + 1;
Transfinite Curve {2, 4, 6, 8} = 4/Mesh.ElementOrder + 1;
Transfinite Curve {9, 10, 11, 12} = 8/Mesh.ElementOrder + 1;

/*
Transfinite Surface {1} = {1, 3, 4, 2};
Transfinite Surface {2} = {5, 7, 8, 6};
Transfinite Surface {3} = {2, 6, 5, 1};
Transfinite Surface {4} = {3, 7, 8, 4};
Transfinite Surface {5} = {4, 8, 6, 2};
Transfinite Surface {6} = {1, 5, 7, 3};
*/
Transfinite Surface "*";
Transfinite Volume "*";
