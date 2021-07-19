// NAFEMS LE10 benchmark structured fully hexahedral mesh
SetFactory("OpenCASCADE");
Merge "le10.step";   // load the CAD

// do some boolean operations to be able to use the transfinite algorithms
Box(2) = {0, 0, 0, 3500, 3500, 500};
BooleanFragments{ Volume{1}; Delete; }{ Volume{2}; Delete; }
Recursive Delete { Volume{3}; }
Coherence;

// define physical names from the geometrical entity ids
Physical Surface("upper") = {9};
Physical Surface("DCD'C'") = {7, 1};
Physical Surface("ABA'B'") = {11, 6};
Physical Surface("BCB'C'") = {10, 5};
Physical Curve("midplane") = {10};
Physical Volume("bulk") = {2, 1};

// meshing settings, read Gmsh' manual for further reference
Mesh.RecombineAll = 1;          // create quads instead of triangs
Mesh.ElementOrder = 2;          // use second-order hexahedra
Mesh.SecondOrderIncomplete = 1; // create hex20 instead of hex27

Mesh.MeshSizeFactor = 0.25;
Transfinite Curve {-1, 3, -14, 6, 11, 18} = 4/Mesh.MeshSizeFactor+1 Using Progression 1.1;
Transfinite Curve {5, 17, 7, 10, 19} = 6/Mesh.MeshSizeFactor+1 Using Progression 1.1;
Transfinite Curve {-9} = 6/Mesh.MeshSizeFactor+1 Using Progression 1.08125;
Transfinite Curve {16, 8, 13, 2, 15, 4, 20, 12} = 1/Mesh.MeshSizeFactor+1 Using Progression 1;
Transfinite Surface "*";
Transfinite Volume "*";
