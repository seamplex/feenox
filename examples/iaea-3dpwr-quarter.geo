SetFactory("OpenCASCADE");
Merge "iaea-3dpwr-quarter.brep";

Physical Volume("fuel1", 1) = {1};
Physical Volume("fuel2", 2) = {4};
Physical Volume("fuel2rod", 3) = {5, 6, 11, 12, 13};
Physical Volume("reflector", 4) = {2};
Physical Volume("reflrod", 5) = {3, 7, 8, 9, 10};

Physical Surface("mirror", 6) = {34, 11, 70, 99, 93, 46, 74, 90, 88, 25, 86, 91, 52, 77, 85, 63, 66, 22};
Physical Surface("vacuum", 7) = {28, 92, 89, 95, 97, 64, 27, 35, 36, 37, 38, 39, 40, 41, 42, 43, 26};
General.SmallAxes = 0;

// meshing options
Mesh.Algorithm = 8;
Mesh.RecombineAll = 0;
Mesh.Optimize = 1;
Mesh.OptimizeNetgen = 1;
Mesh.ElementOrder = 2;

Mesh.MeshSizeMin = 10/Mesh.ElementOrder;
Mesh.MeshSizeMax = 20/Mesh.ElementOrder;

Merge "iaea-colors.geo";
