// NAFEMS LE10 benchmark geometry
SetFactory("OpenCASCADE");

a = 1000;   // geometric parameters (in mm)
b = 2750;
c = 3250;
d = 2000;
h = 600;

Point(1) = {0, a, -h/2};  // the four points A', B', C' and D'
Point(2) = {0, b, -h/2};
Point(3) = {c, 0, -h/2};
Point(4) = {d, 0, -h/2};

Line(1) = {1, 2};         // lower
Ellipse (2) = {0,0,-h/2, c, b, 0, Pi/2};
Line(3) = {3, 4};
Ellipse (4) = {0,0,-h/2, d, a, 0, Pi/2};

Coherence;             // merge the points 

Curve Loop(1) = {3, 4, 1, -2};
Plane Surface(1) = {1};
Extrude {0, 0, h} { Surface{1}; }

Ellipse (13) = {0,0,0, c, b, 0, Pi/2};
BooleanFragments{ Volume{1}; Surface{5}; Delete; }{ Curve{13}; Delete; }

// Print "le10.brep";
Print "le10.step";
