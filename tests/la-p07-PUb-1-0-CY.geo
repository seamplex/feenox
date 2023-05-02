rc = 4.279960;
n = 10;

SetFactory("OpenCASCADE");
Point(1) = {0, 0, 0};
Point(2) = {rc, 0, 0};
Point(3) = {0, rc, 0};

Line(1) = {3, 1};
Line(2) = {1, 2};
Circle(3) = {2, 1, 3};
Curve Loop(1) = {3, 1, 2};
Plane Surface(1) = {1};

Mesh.MeshSizeMax = rc/n;
Mesh.MeshSizeMin = rc/n;
Mesh.ElementOrder = 2;
Mesh.RecombineAll = 0;

Physical Surface("fuel") = {1};
Physical Curve("mirror", 4) = {2, 1};
Physical Curve("vacuum", 5) = {3};
