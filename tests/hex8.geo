SetFactory("OpenCASCADE");

L = 0.5;
b = 0.05;
h = 0.02;

Box(1) = {0,-b/2,-h/2, L, b, h};

Physical Surface("left", 1) = {1};
Physical Surface("right", 2) = {2};
Physical Volume("bulk", 3) = {1};

Mesh.ElementOrder = 1;
Mesh.RecombineAll = 1;

// Transfinite Curve {1, 3, 5, 7} = 4/Mesh.ElementOrder + 1;
// Transfinite Curve {2, 4, 6, 8} = 8/Mesh.ElementOrder + 1;
// Transfinite Curve {9, 10, 11, 12} = 64/Mesh.ElementOrder + 1;

Transfinite Curve {1, 3, 5, 7} = 1 + 1;
Transfinite Curve {2, 4, 6, 8} = 1 + 1;
Transfinite Curve {9, 10, 11, 12} = 1 + 1;

Transfinite Surface "*";
Transfinite Volume "*";


