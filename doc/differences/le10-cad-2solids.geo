// NAFEMS LE10 benchmark geometry
SetFactory("OpenCASCADE");

a = 1000;   // geometric parameters (in mm)
b = 2750;
c = 3250;
d = 2000;
h = 600;

Point(1) = {0, a, 0};  // define the four points A, B, C and D
Point(2) = {0, b, 0};
Point(3) = {c, 0, 0};
Point(4) = {d, 0, 0};


Line(1) = {1, 2};      // join them with the ellipses and straight edges
Ellipse (2) = {0,0,0, c, b, 0, Pi/2};
Line(3) = {3, 4};
Ellipse (4) = {0,0,0, d, a, 0, Pi/2};
Coherence;             // merge the points 

Curve Loop(1) = {1, -2, 3, 4};   // create the bottom surface
Plane Surface(1) = {1};

Extrude {0, 0, 0.5*h} { Surface{1}; }   // extrude it (twice to get the mid-plane edge)
Extrude {0, 0, 0.5*h} { Surface{6}; }

// Print "le10-cad.brep";
Print "le10-cad.step";
