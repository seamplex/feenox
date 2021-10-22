// NAFEMS LE10 benchmark structured locally-refined tetrahedral mesh
// copyright (c) 2021 jeremy theler <jeremy@seamplex.com>

// create the NAFEMS LE10 benchmark geometry CAD
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
Extrude {0, 0, h/2} { Surface{1}; Recombine; }
Extrude {0, 0, h/2} { Surface{6}; Recombine; }
Coherence;


// define physical names from the geometrical entity ids
// don' use apostrophe to make it string-friendly in aster & lizard (just in case)
// set an explicit id for sparselizard
Physical Volume("bulk", 1) = {1,2};
Physical Surface("upper", 2) = {11};
Physical Surface("DCDC", 3) = {2,7};
Physical Surface("ABAB", 4) = {4,9};
Physical Surface("BCBC", 5) = {5,10};
Physical Curve("midplane", 6) = {12};
Physical Curve("DD", 7) = {6,14};
// Physical Point("D", 8) = {8};

// meshing settings, read Gmsh' manual for further reference
Mesh.ElementOrder = 1;
Mesh.RecombineAll = 1;
Mesh.SecondOrderLinear = 1;
Mesh.SecondOrderIncomplete = 1;

Transfinite Curve {4, 9, 17, 2, 12, 20} = 6/Mesh.MeshSizeFactor +1 Using Progression 1.02;
Transfinite Curve {1, 11, 19, -3, -7, -15} = 4/Mesh.MeshSizeFactor +1 Using Progression 1.02;
Transfinite Curve {8, 16, 10, 18, 6, 14, 5, 13} = 1/Mesh.MeshSizeFactor +1;

Transfinite Surface {1} = {4, 1, 2, 3};
Transfinite Surface {2} = {4, 3, 5, 6};
Transfinite Surface {3} = {4, 1, 6, 7};
Transfinite Surface {4} = {1, 2, 7, 8};
Transfinite Surface {5} = {3, 2, 8, 5};
Transfinite Surface {6} = {6, 7, 5, 8};
Transfinite Surface {7} = {6, 5, 9, 10};
Transfinite Surface {8} = {6, 7, 10, 11};
Transfinite Surface {9} = {7, 8, 11, 12};
Transfinite Surface {10} = {5, 8, 12, 9};
Transfinite Surface {11} = {10, 11, 9, 12};

Transfinite Volume{1,2};
