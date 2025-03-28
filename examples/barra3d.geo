SetFactory("OpenCASCADE");
len=1.0;
diam=0.02;
Cylinder(1) = {0, 0, 0, len, 0, 0, diam/2};

Physical Volume("bulk") = {1};
Physical Surface("hot") = {2,3};
Physical Surface("cool") = {1};

lc = diam/8;
Mesh.CharacteristicLengthMax = lc;

Mesh.Algorithm = 6;
Mesh.Optimize = 1;
//Mesh.OptimizeNetgen = 1;

// refinement
// Field[1] = Distance;
// Field[1].FacesList = {3};
// Field[2] = Threshold;
// Field[2].IField = 1;
// Field[2].LcMin = 0.15*lc;
// Field[2].LcMax = lc;
// Field[2].DistMin = 0.2;
// Field[2].DistMax = 2;

// Background Field = {2};

// run:
//      gmsh -3 Barra3D.geo
// to get the mesh file: Barra3D.msh
