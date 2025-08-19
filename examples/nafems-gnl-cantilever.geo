SetFactory("OpenCASCADE");
Box(1) = {0, -0.05, -0.05, 3.2, 0.1, 0.1};

Physical Surface("left") = {1};
Physical Surface("right") = {2};
Physical Volume("steel") = {1};

Transfinite Curve {2, 4, 3, 1, 7, 8, 5, 6} = 4+1;
Transfinite Curve {11, 12, 9, 10} = 4*32+1;
Transfinite Surface {6};
Transfinite Surface {4};
Transfinite Surface {5};
Transfinite Surface {3};
Transfinite Surface {1};
Transfinite Surface {2};
Transfinite Volume{1};

Mesh.RecombineAll = 1;
Mesh.ElementOrder = 2;
// Mesh.SecondOrderIncomplete = 1;
