SetFactory("OpenCASCADE");
l = 20;  // mm
r = 1;   // mm

Point(1) = {0, 0, 0};
Point(2) = {l/2, 0, 0};
Point(3) = {l/2, l/2, 0};
Point(4) = {0, l/2, 0};
Point(5) = {r, 0, 0};
Point(6) = {0, r, 0};



Line(1) = {1, 5};
Line(2) = {5, 2};
Line(3) = {2, 3};
Line(4) = {3, 4};
Line(5) = {4, 6};
Line(6) = {6, 1};
Circle(7) = {5, 1, 6};

Curve Loop(1) = {2, 3, 4, 5, -7};
Plane Surface(1) = {1};
Curve Loop(2) = {1, 7, 6};
Plane Surface(2) = {2};


// physical groups
Physical Surface("bulk", 1) = {1};
Physical Surface("hot", 2) = {2};
Physical Curve("x", 3) = {1, 2};
Physical Curve("y", 4) = {6, 5};


// meshing settings
// Mesh.MeshSizeMin = 10;
// Mesh.MeshSizeMax = 10;

Mesh.MeshSizeMin = r/5;
Mesh.MeshSizeMax = r;
Mesh.Algorithm = 8;
Mesh.RecombineAll = 1;
Mesh.Optimize = 1;
Mesh.OptimizeNetgen = 1;
Mesh.ElementOrder = 1;
Mesh.SecondOrderLinear = 0;

// local refinement around the center (point 1)
Field[1] = Distance;
Field[1].NodesList = {1};
Field[2] = Threshold;
Field[2].IField = 1;
Field[2].LcMin = Mesh.MeshSizeMin;
Field[2].LcMax = Mesh.MeshSizeMax;
Field[2].DistMin = 1.5*r;
Field[2].DistMax = 5*r;
Background Field = {2};
