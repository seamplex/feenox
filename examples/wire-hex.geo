l = 0.5*303e-3;
d = 1.948e-3;

Point(1) = {0, 0, 0};
Point(2) = {d/2, 0, 0};
Point(3) = {0, d/2, 0};
Point(4) = {0, -d/2, 0};
Point(5) = {-d/2, 0, 0};
Circle(1) = {3, 1, 2};
Circle(2) = {2, 1, 4};
Circle(3) = {4, 1, 5};
Circle(4) = {5, 1, 3};

Point(6) = {d/4, 0, 0};
Point(7) = {0, d/4, 0};
Point(8) = {0, -d/4, 0};
Point(9) = {-d/4, 0, 0};

Line(5) = {6, 7};
Line(6) = {7, 9};
Line(7) = {9, 8};
Line(8) = {8, 6};
Line(9) = {6, 2};
Line(10) = {7, 3};
Line(11) = {9, 5};
Line(12) = {8, 4};

Curve Loop(1) = {5, 10, 1, -9};
Plane Surface(1) = {1};
Curve Loop(2) = {10, -4, -11, -6};
Plane Surface(2) = {2};
Curve Loop(3) = {11, -3, -12, -7};
Plane Surface(3) = {3};
Curve Loop(4) = {12, -2, -9, -8};
Plane Surface(4) = {4};
Curve Loop(5) = {5, 6, 7, 8};
Plane Surface(5) = {5};

Transfinite Curve{1:8} = 4;
Transfinite Curve{9:12} = 2;
Transfinite Surface{:};
Recombine Surface{:};
Extrude {0, 0, l} { Surface{:}; Layers{80}; Recombine; }

Mesh.ElementOrder = 2;
Mesh.SecondOrderIncomplete = 1;

Physical Surface("fixed") = {1, 2, 3, 4, 5};
Physical Volume("bulk") = {1, 2, 3, 3, 4, 5};
