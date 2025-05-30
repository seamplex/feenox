SetFactory("OpenCASCADE");
a = 1;
n = 1;
Box(1) = {0, 0, 0, a, a, a};

Physical Surface("left",1) = {1};
Physical Surface("right",2) = {2};
Physical Surface("front",3) = {3};
Physical Surface("back",4) = {4};
Physical Surface("bottom",5) = {5};
Physical Surface("top",6) = {6};
Physical Volume("volume",7) = {1};

Mesh.ElementOrder = 1;
Mesh.RecombineAll = 1;
Mesh.Recombine3DAll = 1;

Transfinite Line "*" = 1;
Transfinite Surface "*";
Transfinite Volume "*";
