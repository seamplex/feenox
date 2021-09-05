Point(1) = {0,   0, 0};
Point(2) = {1,   0, 0};
Line(1) = {1, 2};

Mesh.MeshSizeMin = 1/(60-1);
Mesh.MeshSizeMax = 1/(60-1);

Physical Point("left") = {1};
Physical Point("right") = {2};
Physical Line("bulk") = {1};
