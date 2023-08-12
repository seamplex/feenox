// // unstructured curved tet10
Mesh.RecombineAll = 0;
Mesh.SecondOrderLinear = 0;

SetFactory("OpenCASCADE");

l = 2*(( 323.8/2    )-( ( 323.8/2    )-21.5     ));

Cylinder(1) = {-l/2, 0, 0,  +l, 0, 0,  ( 323.8/2    )};
Cylinder(2) = {-l/2, 0, 0,  +l, 0, 0,  ( ( 323.8/2    )-21.5     )};
BooleanDifference(3) = {Volume{1};Delete;}{Volume{2};Delete;};

Physical Volume("pipe") = {3};
s() = Boundary{ Volume{3}; };
Physical Surface("front") = {s(1)};
Physical Surface("back") = {s(2)};
Physical Surface("pressure") = {s(3)};

// Mesh.Algorithm = 6;
// Mesh.Algorithm3D = 4;
// Mesh.Optimize = 1;
// Mesh.OptimizeNetgen = 1;
// Mesh.HighOrderOptimize = 2;

Mesh.CharacteristicLengthMax = (( 323.8/2    )-( ( 323.8/2    )-21.5     ))/2;
Mesh.ElementOrder = 2;

