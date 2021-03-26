SetFactory("OpenCASCADE");
Rectangle(1) = {0, 0, 0, 1, 1, 0};
Rectangle(2) = {1, 0, 0, 1, 1, 0};
Coherence;

Mesh.MeshSizeMax = 0.5;
Mesh.MeshSizeMin = 0.5;

Physical Curve("left", 1) = {4};
Physical Curve("right", 2) = {6};
Physical Surface("soft", 3) = {1};
Physical Surface("hard", 4) = {2};
