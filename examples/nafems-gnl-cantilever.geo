SetFactory("OpenCASCADE");
l = 3.2;
h = 0.1;
Box(1) = {0, -h/2, -h/2, l, h, h};

Physical Surface("left") = {1};
Physical Surface("right") = {2};
Physical Volume("steel") = {1};

n = 2;  // number of elements across thickness
Transfinite Curve {2, 4, 3, 1, 7, 8, 5, 6} = n+1;
Transfinite Curve {11, 12, 9, 10} = n*32+1;
Transfinite Surface "*";
Transfinite Volume "*";

Mesh.RecombineAll = 1;            // quads & hexes
Mesh.ElementOrder = 2;            // second order
Mesh.SecondOrderIncomplete = 1;   // hex20 (and quad8) instead of hex27 (quad9)
