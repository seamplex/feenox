SetFactory("OpenCASCADE");

l = 500;
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


Line(1) = {3, 9};
Line(2) = {9, 11};
Line(3) = {11, 7};
Line(4) = {7, 5};
Line(5) = {5, 1};
Line(6) = {1, 2};
Line(7) = {2, 6};
Line(8) = {6, 8};
Line(9) = {8, 12};
Line(10) = {12, 10};
Line(11) = {10, 4};
Line(12) = {4, 3};

Curve Loop(1) = {3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 1, 2};
Plane Surface(1) = {1};

Extrude {l, 0, 0} {
  Surface{1}; 
}

Physical Volume("bulk", 1) = {1};
Physical Surface("left", 2) = {1};
Physical Surface("right", 3) = {14};
Physical Surface("top", 4) = {11};

Mesh.ElementOrder = 2;
Mesh.MeshSizeMin = 2*t;
Mesh.MeshSizeMax = 2*t;
Mesh.Optimize = 1;
Mesh.OptimizeNetgen = 1;
