SetFactory("OpenCASCADE");
Disk(1) = {0, 0, 0, 1};
Mesh.MeshSizeMax = 0.5;

Physical Curve("perimeter", 2) = {1};
Physical Surface("surface", 3) = {1};
