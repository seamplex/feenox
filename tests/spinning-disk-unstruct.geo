Mesh.Algorithm = 6;
// do not use HXT (Mesh.Algorithm3D = 10): combined with HighOrderOptimize,
// gmsh 4.12 (the one in GitHub's Ubuntu runners) sometimes writes nodes with nan coordinates
Mesh.Algorithm3D = 1;
// the meshing (in particular the optimization) depends on the number of threads,
// so use only one to get the same mesh regardless of the number of cores
General.NumThreads = 1;
Mesh.MeshSizeMax = R/16;
Mesh.ElementOrder = 2;
Mesh.SecondOrderLinear = 0;
Mesh.Optimize = 1;
//Mesh.OptimizeNetgen = 1;
Mesh.HighOrderOptimize = 1;
