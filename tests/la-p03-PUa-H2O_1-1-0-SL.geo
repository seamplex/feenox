rc = 1.478401;
h_refl = 3.063725;
n = 20;

SetFactory("OpenCASCADE");
Point(1) = {-rc, 0, 0};
Point(2) = {+rc, 0, 0};
Point(3) = {+rc+h_refl, 0, 0};
Line(1) = {1, 2};
Line(2) = {2, 3};

Physical Curve("fuel") = {1};
Physical Curve("refl") = {2};
Physical Point("vacuum") = {1,3};

Mesh.MeshSizeMax = rc/n;
Mesh.MeshSizeMin = 0.5*rc/n;
Mesh.ElementOrder = 2;
