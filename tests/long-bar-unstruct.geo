SetFactory("OpenCASCADE");

a = 2;
n = 4;
Box(1) = {0,0,0, a,a,10*a};

Physical Volume("bulk") = {1};
Physical Surface("rear") = {5};
Physical Surface("front") = {6};

Mesh.MeshSizeMax = a/n;
Mesh.ElementOrder = 2;
Mesh.SecondOrderLinear = 1;
