a = 100;
b = 40;
c = 40;

lc = 2;
Mesh.MeshSizeMin = lc;
Mesh.MeshSizeMax = lc;

Point(1) = { -b, 0, 0};
Point(2) = {  0, 0, 0};
Point(3) = {  a, 0, 0};
Point(4) = {a+c, 0, 0};

Line(1) = {1, 2};
Line(2) = {2, 3};
Line(3) = {3, 4};

Physical Line("left_refl") = {1};
Physical Line("fuel") = {2};
Physical Line("right_refl") = {3};

Physical Point("left") = {1};
Physical Point("right") = {4};
