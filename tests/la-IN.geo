SetFactory("OpenCASCADE");
Point(1) = {0, 0, 0};
Point(2) = {1, 0, 0};
Line(1) = {1, 2};



n = 10;
Mesh.MeshSizeMax = 1/n;
Mesh.MeshSizeMin = 1/n;

Physical Curve("fuel") = {1};
Physical Point("mirror") = {1,2};
