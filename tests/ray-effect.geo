SetFactory("OpenCASCADE");
Rectangle(1) = {0, 0, 0, 1, 1, 0};
Rectangle(2) = {0, 0, 0, 2, 2, 0};
Coherence;

n = 3;
Transfinite Curve {7, 1, 2, 8} = 1+n;
Transfinite Curve {3, 6} = 1+n;
Transfinite Curve {4, 5} = 1+2*n;
Mesh.Algorithm = 8;
Mesh.RecombineAll = 1;
Mesh.ElementOrder = 2;
Mesh.SecondOrderIncomplete = 0;


Physical Surface("src") = {1};
Physical Surface("bulk") = {2};
Physical Curve("mirror") = {3, 8, 7, 6};
Physical Curve("vacuum") = {4, 5};
