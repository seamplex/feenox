SetFactory("OpenCASCADE");
l = 20;  // mm
r = 1;   // mm

Point(1) = {0, 0, 0};
Point(2) = {l/2, 0, 0};
Point(3) = {l/2, l/2, 0};
Point(4) = {0, l/2, 0};
Point(5) = {r, 0, 0};
Point(6) = {0, r, 0};


Point(7) = {0.5*r, 0, 0};
Point(8) = {0, 0.5*r, 0};
Point(9) = {0.4*r, 0.4*r, 0};
Point(10) = {r*Sin(Pi/4), r*Sin(Pi/4), 0};

Line(1) = {1, 7};
Line(2) = {7, 5};
Line(3) = {5, 2};
Line(4) = {2, 3};
Line(5) = {3, 4};
Line(6) = {4, 6};
Line(7) = {6, 8};
Line(8) = {8, 1};
Line(9) = {7, 9};
Line(10) = {9, 8};
Line(11) = {9, 10};
Line(12) = {10, 3};
Circle(13) = {5, 1, 10};
Circle(14) = {10, 1, 6};

Curve Loop(1) = {3, 4, -12, -13};
Plane Surface(1) = {1};
Curve Loop(2) = {5, 6, -14, 12};
Plane Surface(2) = {2};
Curve Loop(3) = {1, 9, 10, 8};
Plane Surface(3) = {3};
Curve Loop(4) = {2, 13, -11, -9};
Plane Surface(4) = {4};
Curve Loop(5) = {14, 7, -10, 11};
Plane Surface(5) = {5};

Transfinite Curve {1, 9, 10, 8, 2, 13, 11, 9, 14, 7, 10, 13, 4, 5, 14} = 2*8+1;
Transfinite Curve {3, 12, -6} = 2*24+1 Using Progression 1.15;

Transfinite Surface {3} = {1, 7, 9, 8};
Transfinite Surface {4} = {7, 5, 10, 9};
Transfinite Surface {5} = {8, 9, 10, 6};
Transfinite Surface {1} = {5, 2, 3, 10};
Transfinite Surface {2} = {10, 3, 4, 6};

Mesh.ElementOrder = 2;
Mesh.RecombineAll = 1;

// physical groups
Physical Surface("bulk") = {1, 2};
Physical Surface("hot") = {3, 4, 5};
Physical Curve("x") = {1, 2, 3};
Physical Curve("y") = {8, 7, 6};
