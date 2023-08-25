// --- geometry -------------------------------------------------
SetFactory("OpenCASCADE");
a = 5;
b = 10;
Rectangle(1) = {0, 0, 0, a, a};
Rectangle(2) = {a, a, 0, a, a};
Rectangle(3) = {0, a, 0, a, a};
Rectangle(4) = {a, 0, 0, a, a};
Coherence;

// --- physical groups ------------------------------------------
Physical Surface("llq",1) = {1};
Physical Surface("lrq",2) = {4};
Physical Surface("urq",3) = {2};
Physical Surface("ulq",4) = {3};

Physical Curve("mirror", 13) = {10, 4, 1, 11};
Physical Curve("vacuum", 14) = {9, 7, 6, 12};

Transfinite Curve "*" = 1+16;
Transfinite Surface "*";
Mesh.RecombineAll = 1;
Mesh.ElementOrder = 2;
Mesh.SecondOrderIncomplete = 0;
