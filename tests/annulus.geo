SetFactory("OpenCASCADE");

a = 508;
b = 1016;
h = 25.4;
n = 8;

Point(1) = {0,0,0};

Point(2) = {a/2,0,0};
Point(3) = {0,a/2,0};
Point(4) = {-a/2,0,0};
Point(5) = {0,-a/2,0};

Point(6) = {b/2,0,0};
Point(7) = {0,b/2,0};
Point(8) = {-b/2,0,0};
Point(9) = {0,-b/2,0};

Circle(1) = {2, 1, 3};
Circle(2) = {3, 1, 4};
Circle(3) = {4, 1, 5};
Circle(4) = {5, 1, 2};
Circle(5) = {6, 1, 7};
Circle(6) = {7, 1, 8};
Circle(7) = {8, 1, 9};
Circle(8) = {9, 1, 6};

Line(9) = {2, 6};
Line(10) = {3, 7};
Line(11) = {4, 8};
Line(12) = {5, 9};

Curve Loop(1) = {9, 5, -10, -1};
Plane Surface(1) = {1};

Curve Loop(2) = {10, 6, -11, -2};
Plane Surface(2) = {2};

Curve Loop(3) = {11, 7, -12, -3};
Plane Surface(3) = {3};

Curve Loop(4) = {12, 8, -9, -4};
Plane Surface(4) = {4};

Transfinite Curve {1, 2, 3, 4, 5, 6, 7, 8} = 1+2*n;
Transfinite Curve {10, 11, 12, 9} = 1+n;

Transfinite Surface {1} = {2, 6, 7, 3};
Transfinite Surface {2} = {3, 7, 8, 4};
Transfinite Surface {3} = {8, 9, 5, 4};
Transfinite Surface {4} = {9, 6, 2, 5};

Mesh.RecombineAll = 1;
Extrude {0, 0, h} {
  Surface{1}; Surface{2}; Surface{3}; Surface{4}; Layers {n/4}; Recombine;
}

Mesh.ElementOrder = 2;
Mesh.SecondOrderIncomplete = 0;

Physical Volume("bulk") = {1, 2, 3, 4};
Physical Surface("inner") = {12, 16, 19, 8};
