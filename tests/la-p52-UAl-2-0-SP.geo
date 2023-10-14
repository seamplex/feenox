rc = 17.66738;
n = 7;

SetFactory("OpenCASCADE");
Sphere(1) = {0, 0, 0, rc, 0, Pi/2, Pi/2};

Mesh.MeshSizeMax = rc/n;
Mesh.MeshSizeMin = rc/n;
Mesh.ElementOrder = 1;
Mesh.RecombineAll = 0;
Mesh.Optimize = 1;
//Mesh.OptimizeNetgen = 1;

Physical Volume("fuel") = {1};
Physical Surface("mirror") = {4, 3, 2};
Physical Surface("vacuum") = {1};
