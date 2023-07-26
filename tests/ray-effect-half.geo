SetFactory("OpenCASCADE");
Rectangle(1) = {0, -1, 0, 1, 2, 0};
Rectangle(2) = {0, -2, 0, 2, 4, 0};
Coherence;

n = 3;
Transfinite Curve {1, 2, 4, 5} = 1+n;
Transfinite Curve {3, 9, 8, 6} = 1+2*n;
Transfinite Curve {7} = 1+4*n;
Mesh.Algorithm = 8;
Mesh.RecombineAll = 1;
Mesh.ElementOrder = 2;
Mesh.SecondOrderIncomplete = 0;


Physical Surface("src") = {1};
Physical Surface("bulk") = {2};
Physical Curve("mirror") = {5, 9, 1};
Physical Curve("vacuum") = {6, 7, 8};
