SetFactory("OpenCASCADE");
Cylinder(1) = {0, 0, 0, 1, 0, 0, 0.5};

Physical Volume("bulk") = {1};
Physical Surface("hot") = {3};
Physical Surface("cool") = {1, 2};

lc = 0.35;
Mesh.CharacteristicLengthMax = lc;

Mesh.Algorithm = 6;
Mesh.Optimize = 1;
Mesh.OptimizeNetgen = 1;

// refinement
Field[1] = Distance;
Field[1].FacesList = {3};
Field[2] = Threshold;
Field[2].IField = 1;
Field[2].LcMin = 0.15*lc;
Field[2].LcMax = lc;
Field[2].DistMin = 0.2;
Field[2].DistMax = 2;

Background Field = {2};
