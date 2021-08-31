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

Coherence;                // merge the points 

Curve Loop(1) = {3, 4, 1, -2};
Plane Surface(1) = {1};
Extrude {0, 0, h} { Surface{1}; }

Ellipse (13) = {0,0,0, c, b, 0, Pi/2};  // midplane
BooleanFragments{ Volume{1}; Surface{5}; Delete; }{ Curve{13}; Delete; }

// define physical names from the geometrical entity ids
Physical Surface("upper") = {7};
Physical Surface("DCD'C'") = {3};
Physical Surface("ABA'B'") = {5};
Physical Surface("BCB'C'") = {1, 2};
Physical Curve("midplane") = {13};
Physical Volume("bulk") = {1};

Mesh.ElementOrder = 2;      // use second-order tetrahedra
Mesh.Algorithm = 6;         // 2D mesh algorithm
Mesh.Algorithm3D = 1 ;      // 3D mesh algorithm
Mesh.Optimize = 1;          // Optimize the mesh
Mesh.HighOrderOptimize = 2; // Optimize high-order meshes? 2: elastic+optimization

Mesh.MeshSizeMax = h/2;     // main element size 
Mesh.MeshSizeMin = h/6;     // refined element size

// local refinement around the point D (entity 8)
Field[1] = Distance;
Field[1].NodesList = {8};
Field[2] = Threshold;
Field[2].IField = 1;
Field[2].LcMin = Mesh.MeshSizeMin;
Field[2].LcMax = Mesh.MeshSizeMax;
Field[2].DistMin = 0.5 * Mesh.MeshSizeMax;
Field[2].DistMax = 1.0 * Mesh.MeshSizeMax;
Background Field = {2};

