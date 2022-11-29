SetFactory("OpenCASCADE");

a = 2;
n = 4;
Box(1) = {0,0,0, a,a,10*a};

Physical Volume("bulk") = {1};
Physical Surface("rear") = {5};
Physical Surface("front") = {6};

Mesh.ElementOrder = 2;
Mesh.SecondOrderIncomplete = 2;
Mesh.RecombineAll = 1;
Mesh.Recombine3DAll = 1;

Transfinite Line "*" = n+1;
Transfinite Line {1,3,5,7} = 10*n+1;
Transfinite Surface "*";
Transfinite Volume "*";
