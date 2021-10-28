// NAFEMS LE10 benchmark unstructured locally-refined tetrahedral mesh
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
Extrude {0, 0, h} { Surface{1}; }

Ellipse (13) = {0,0,0, c, b, 0, Pi/2};
BooleanFragments{ Volume{1}; Surface{5}; Delete; }{ Curve{13}; Delete; }


// define physical names from the geometrical entity ids
// don' use apostrophe to make it string-friendly in aster & lizard (just in case)
// set an explicit id for sparselizard
Physical Volume("bulk", 1) = {1};
Physical Surface("upper", 2) = {7};
Physical Surface("DCDC", 3) = {3};
Physical Surface("ABAB", 4) = {5};
Physical Surface("BCBC", 5) = {1, 2};
Physical Curve("midplane", 6) = {13};
Physical Curve("DD", 7) = {20};
// Physical Point("D", 8) = {8};   // code aster complains about this group

// meshing settings, read Gmsh' manual for further reference
Mesh.ElementOrder = 2;      // use second-order
Mesh.SecondOrderLinear = 0; // curved tetrahedra

Mesh.Algorithm = 6;         // 2D mesh algorithm:  6: Frontal Delaunay
Mesh.Algorithm3D = 1;       // 3D mesh algorithm: (1: Delaunay, 3: Initial mesh only, 4: Frontal, 7: MMG3D, 9: R-tree, 10: HXT)

Mesh.Optimize = 1;          // Optimize the mesh
Mesh.OptimizeNetgen = 1;    // Optimize the mesh
Mesh.HighOrderOptimize = 1; // Optimize high-order meshes? 2: elastic+optimization

Mesh.MeshSizeMax = 300;     // main element size 
Mesh.MeshSizeMin = 100;     // refined element size

// local refinement around the point D (entity 8)
Field[1] = Distance;
Field[1].NodesList = {8};
Field[2] = Threshold;
Field[2].IField = 1;
Field[2].LcMin = 100;
Field[2].LcMax = 300;
Field[2].DistMin = Mesh.MeshSizeMin;
Field[2].DistMax = Mesh.MeshSizeMax;
Background Field = {2};
