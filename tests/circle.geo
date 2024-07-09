SetFactory("OpenCASCADE");
Disk(1) = {0, 0, 0, 1};
Mesh.MeshSizeMax = 1;

Physical Curve("perimeter", 2) = {1};
Physical Surface("surface", 3) = {1};
