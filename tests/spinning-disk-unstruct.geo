Mesh.Algorithm = 6;
Mesh.Algorithm3D = 10;
// HXT is multithreaded and the resulting mesh depends on the number of threads,
// gmsh 4.12 even writes nodes with nan coordinates now and then (as in GitHub's runners),
// so force a single thread to get a reproducible mesh regardless of the number of cores
General.NumThreads = 1;
Mesh.MeshSizeMax = R/16;
Mesh.ElementOrder = 2;
Mesh.SecondOrderLinear = 0;
Mesh.Optimize = 1;
//Mesh.OptimizeNetgen = 1;
Mesh.HighOrderOptimize = 1;
