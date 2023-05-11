rc = 3.077574;
h_refl = 30.637255;
n = 6;

SetFactory("OpenCASCADE");
Point(1) = {0, 0, 0};
Point(2) = {rc, 0, 0};
Point(3) = {0, rc, 0};
Point(4) = {rc+h_refl, 0, 0};
Point(5) = {0, rc+h_refl, 0};

Line(1) = {3, 1};
Line(2) = {1, 2};
Circle(3) = {2, 1, 3};
Curve Loop(1) = {3, 1, 2};
Plane Surface(1) = {1};

Line(4) = {2, 4};
Line(5) = {5, 3};
Circle(6) = {4, 1, 5};
Curve Loop(2) = {4, 6, 5, -3};
Plane Surface(2) = {2};


Physical Surface("fuel") = {1};
Physical Surface("refl") = {2};
Physical Curve("mirror", 7) = {5, 1, 2, 4};
Physical Curve("vacuum", 8) = {6};


Mesh.MeshSizeMax = rc/(0.5*n);
Mesh.MeshSizeMin = rc/(2*n);
Mesh.Optimize = 1;
//Mesh.OptimizeNetgen = 1;
// 8 does not work in ubuntu (why not?)
//Mesh.Algorithm = 8;
Mesh.ElementOrder = 2;
Mesh.RecombineAll = 0;


// local refinements
Field[1] = Distance;
Field[1].EdgesList = {3};
Field[2] = Threshold;
Field[2].IField = 1;
Field[2].LcMin = Mesh.MeshSizeMin;
Field[2].LcMax = Mesh.MeshSizeMax;
Field[2].DistMin = 4*Mesh.MeshSizeMin;
Field[2].DistMax = 8*Mesh.MeshSizeMin;
Background Field = {2};


