SetFactory("OpenCASCADE");

// read the BREP
Merge "valve.brep";

// meshing settings
Mesh.ElementOrder = 1;
Mesh.Optimize = 1;
Mesh.OptimizeNetgen = 1;
Mesh.Algorithm = 6;     //  (1=MeshAdapt, 2=Automatic, 5=Delaunay, 6=Frontal, 7=BAMG, 8=DelQuad)
Mesh.Algorithm3D = 10;  //  (1=Delaunay, 2=New Delaunay, 4=Frontal, 5=Frontal Delaunay, 6=Frontal Hex, 7=MMG3D, 9=R-tree)

Mesh.CharacteristicLengthMax = 24;
Mesh.CharacteristicLengthMin = 0.1*Mesh.CharacteristicLengthMax;

// local refinement
Field[1] = Distance;
Field[1].FacesList = {16};

Field[2] = Threshold;
Field[2].IField = 1;
Field[2].LcMin = Mesh.CharacteristicLengthMin;
Field[2].LcMax = Mesh.CharacteristicLengthMax;
Field[2].DistMin = 5;
Field[2].DistMax = 130;

Background Field = 2;

// carbon steel
Physical Volume("CS", 1) = {2};
// stainless stell
Physical Volume("SS", 2) = {1,3,4};

// bcs
Physical Surface("symmetry", 3) = {3, 4, 15, 17, 20, 25, 91, 110, 111};
Physical Surface("internal", 4) = {7, 8, 9, 10, 18, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 60, 61, 62, 63, 64, 65, 66, 67, 68, 97, 98, 99, 100, 101, 112};
Physical Surface("external", 5) = {1, 2, 6, 12, 13, 19, 21, 22, 23, 24, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 54, 55, 56, 57, 58, 59, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 92, 93, 94, 95, 96, 102, 103, 104, 105, 106, 107, 108};

// these are needed to compute the mean value
Physical Surface("end_carbon", 6) = {14};
Physical Surface("end_ss", 7) = {109};
