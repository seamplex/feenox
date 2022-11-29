SetFactory("OpenCASCADE");
Rectangle(1) = {0, 0, 0, 1.5, 1.5, 0};
Rectangle(2) = {1.5, 1.5, 0, 1.5, 1.5, 0};
Rectangle(3) = {0, 1.5, 0, 1.5, 1.5, 0};
Rectangle(4) = {1.5, 0, 0, 1.5, 1.5, 0};
Coherence;

Physical Curve("left") = {4,10};
Physical Curve("right") = {6,12};
Physical Curve("bottom") = {1,11};
Physical Curve("top") = {9,7};
Physical Surface("quadrant1") = {1};
Physical Surface("quadrant2") = {2};
Physical Surface("quadrant3") = {3};
Physical Surface("quadrant4") = {4};

Transfinite Curve "*" = 8+1;
Transfinite Surface "*";
Mesh.RecombineAll = 1;
