SetFactory("OpenCASCADE");
Merge "iaea-3dpwr-eighth.step";
Coherence;

Physical Volume("fuel1", 1) = {1};
Physical Volume("fuel2", 2) = {4};
Physical Volume("fuel2rod", 3) = {5, 6, 10, 11};
Physical Volume("reflector", 4) = {2};
Physical Volume("reflrod", 5) = {3, 7, 8, 9};

Physical Surface("mirror", 6) = {37, 14, 66, 67, 39, 53, 63, 68, 38, 60, 57, 58, 56, 19, 48, 42, 54, 36, 52, 3};
Physical Surface("vacuum", 7) = {59, 33, 61, 64, 41, 34, 26, 25, 24, 23, 35};

// meshing options
Mesh.Algorithm = 6;  // (1: MeshAdapt, 2: Automatic, 3: Initial mesh only, 5: Delaunay, 6: Frontal-Delaunay, 7: BAMG, 8: Frontal-Delaunay for Quads, 9: Packing of Parallelograms, 11: Quasi-structured Quad)
Mesh.RecombineAll = 0;
Mesh.Optimize = 1;
Mesh.OptimizeNetgen = 1;
Mesh.ElementOrder = 2;

Mesh.MeshSizeMin = 10/Mesh.ElementOrder;
Mesh.MeshSizeMax = 20/Mesh.ElementOrder;

Merge "iaea-colors.geo";
