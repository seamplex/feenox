SetFactory("OpenCASCADE");
Rectangle(1) = {-1, -1, 0, 2, 2, 0};
Rectangle(2) = {-2, -2, 0, 4, 4, 0};
Coherence;

n = 3;
Transfinite Curve {1, 2, 4, 3} = 1+2*2*n;
Transfinite Curve {5, 6, 7, 8} = 1+2*n;

Mesh.Algorithm = 8;
Mesh.RecombineAll = 1;
Mesh.ElementOrder = 2;
Mesh.SecondOrderIncomplete = 0;

Physical Surface("src") = {1};
Physical Surface("bulk") = {2};
Physical Curve("vacuum") = {1, 3, 4, 2};
