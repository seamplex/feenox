SetFactory("OpenCASCADE");

b = 1;     // cylinder radius
h = 0.5;   // cylinder height 

f = 0.4;   // fraction of internal square length
n = 4;     // number of elements per transfinite line

Rectangle(1) = {-f*b, -f*b, 0, 2*f*b, 2*f*b, 0};
Rotate {{0, 0, 1}, {0, 0, 0}, Pi/4} {
  Surface{1}; 
}

Point(5) = {b, 0, 0};
Point(6) = {0, b, 0};
Point(7) = {-b, 0, 0};
Point(8) = {0, -b, 0};
Point(9) = {0, 0, 0};


Line(5) = {2, 5};
Line(6) = {3, 6};
Line(7) = {4, 7};
Line(8) = {1, 8};

Circle(9) = {8, 9, 5};
Circle(10) = {5, 9, 6};
Circle(11) = {6, 9, 7};
Circle(12) = {7, 9, 8};

Curve Loop(2) = {10, -6, -2, 5};
Plane Surface(2) = {2};

Curve Loop(3) = {11, -7, -3, 6};
Plane Surface(3) = {3};

Curve Loop(4) = {12, -8, -4, 7};
Plane Surface(4) = {4};

Curve Loop(5) = {9, -5, -1, 8};
Plane Surface(5) = {5};


Mesh.RecombineAll = 1;

Transfinite Curve {8, 5, 6, 7} = 1+n;
Transfinite Curve {1, 12, 2, 9, 3, 10, 4, 11} = 1+n;
Transfinite Surface {:};


Extrude {0, 0, h} {
  Surface{1:5}; Layers {n}; Recombine;
}

Physical Surface("inf", 1) = {4, 5, 2, 3, 1};
Physical Volume("bulk", 2) = {1, 3, 4, 5, 2};

Mesh.ElementOrder = 2;
Mesh.SecondOrderLinear = 0;
