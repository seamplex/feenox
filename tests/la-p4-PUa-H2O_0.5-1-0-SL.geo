rc = 1.317831;
h_refl = 1.531863;
n = 20;

SetFactory("OpenCASCADE");
Point(1) = {0, 0, 0};
Point(2) = {+rc, 0, 0};
Point(3) = {+rc+h_refl, 0, 0};
Line(1) = {1, 2};
Line(2) = {2, 3};

Mesh.MeshSizeMax = rc/n;
Mesh.MeshSizeMin = 0.5*rc/n;
Mesh.ElementOrder = 2;

Physical Curve("fuel") = {1};
Physical Curve("refl") = {2};
Physical Point("mirror") = {1};
Physical Point("vacuum") = {3};
