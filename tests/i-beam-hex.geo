SetFactory("OpenCASCADE");

n = 1/Mesh.MeshSizeFactor;

l = 1000;
h = 80;
w = 60;
t = 10;

Point(1)  = {0, +w/2, -h/2};
Point(2)  = {0, -w/2, -h/2};

Point(3)  = {0, +w/2, +h/2};
Point(4)  = {0, -w/2, +h/2};

Point(5)  = {0, +w/2, -h/2+t};
Point(6)  = {0, -w/2, -h/2+t};
Point(7)  = {0, +t,   -h/2+t};
Point(8)  = {0, -t,   -h/2+t};

Point(9)  = {0, +w/2, +h/2-t};
Point(10) = {0, -w/2, +h/2-t};
Point(11) = {0, +t,   +h/2-t};
Point(12) = {0, -t,   +h/2-t};

Point(13)  = {0, +t,   -h/2};
Point(14)  = {0, +t,   +h/2};
Point(15)  = {0, -t,   -h/2};
Point(16)  = {0, -t,   +h/2};

Line(1) = {2, 15};
Line(2) = {15, 13};
Line(3) = {13, 1};
Line(4) = {1, 5};
Line(5) = {5, 7};
Line(6) = {7, 8};
Line(7) = {8, 6};
Line(8) = {6, 2};
Line(9) = {9, 11};
Line(10) = {11, 12};
Line(11) = {12, 10};
Line(12) = {10, 4};
Line(13) = {4, 16};
Line(14) = {16, 14};
Line(15) = {14, 3};
Line(16) = {3, 9};
Line(17) = {13, 7};
Line(18) = {7, 11};
Line(19) = {11, 14};
Line(20) = {15, 8};
Line(21) = {8, 12};
Line(22) = {12, 16};


Curve Loop(1) = {3, 4, 5, -17};
Plane Surface(1) = {1};

Curve Loop(2) = {2, 17, 6, -20};
Plane Surface(2) = {2};

Curve Loop(3) = {1, 20, 7, 8};
Plane Surface(3) = {3};

Curve Loop(4) = {21, -10, -18, 6};
Plane Surface(4) = {4};

Curve Loop(5) = {11, 12, 13, -22};
Surface(5) = {5};

Curve Loop(7) = {14, -19, 10, 22};
Surface(6) = {7};

Curve Loop(9) = {15, 16, 9, 19};
Surface(7) = {9};

Extrude {l, 0, 0} {
  Surface{1}; Surface{2}; Surface{3}; Surface{4}; Surface{7}; Surface{6}; Surface{5}; Layers {16*n}; Recombine;
}


Physical Volume("bulk", 1) = {1,2,3,4,5,6,7};
Physical Surface("left", 2) = {1, 2, 3, 4, 5, 6, 7};
Physical Surface("right", 3) = {12, 16, 20, 24, 29, 32, 36};
Physical Surface("top", 4) = {33, 31, 28};

Mesh.ElementOrder = 2;
Mesh.RecombineAll = 1;
Mesh.SecondOrderIncomplete = 0;

Transfinite Curve {1, 7, 3, 5, 9, 15, 11, 13} = 2*n+1;
Transfinite Curve {8, 20, 17, 4, 12, 22, 19, 16} = 1*n+1;
Transfinite Curve {2, 6, 10, 14} = 2*n+1;
Transfinite Curve {21, 18} = 4*n+1;

Transfinite Surface {1} = {13, 7, 5, 1};
Transfinite Surface {2} = {15, 8, 7, 13};
Transfinite Surface {3} = {2, 6, 8, 15};
Transfinite Surface {4} = {8, 12, 11, 7};
Transfinite Surface {5} = {10, 4, 16, 12};
Transfinite Surface {6} = {12, 16, 14, 11};
Transfinite Surface {7} = {11, 14, 3, 9};


