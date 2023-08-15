// SetFactory("OpenCASCADE");
// Box(1) = {-28, -50, 0, 115, 100, 115};

// https://upload.wikimedia.org/wikipedia/commons/4/43/Stanford_Bunny.stl
Merge "Stanford_Bunny.stl";
Merge "box.stl";

angle = 40;
includeBoundary = 1;
forceParametrizablePatches = 0;
curveAngle = 180;

ClassifySurfaces{angle * Pi/180, includeBoundary, forceParametrizablePatches, curveAngle * Pi / 180};
CreateGeometry;

Surface Loop(1) = {4, 3, 6, 8, 17, 7, 9, 10, 18, 19, 12, 13, 20, 11, 5, 16, 14, 15};
Volume(1) = {1};
Surface Loop(2) = {26, 21, 23, 25, 22, 24};
Volume(2) = {1, 2};

Physical Volume("bunny") = {1};
Physical Volume("box") = {2};
Physical Surface("vacuum", 64) = {4, 21, 26, 24, 22, 3, 25, 23};

Mesh.MeshSizeMin = 8;
Mesh.MeshSizeMax = 15;
// Mesh.ElementOrder = 1;
Mesh.Algorithm = 6;
Mesh.Algorithm3D = 1;
Mesh.Optimize = 1;
Mesh.MeshSizeExtendFromBoundary = 0;
