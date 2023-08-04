Point(1) = {0, 0, 0};
Point(2) = {1, 0, 0};
Line(1) = {1, 2};

Physical Point("left") = {1};
Physical Point("right") = {2};
Physical Line("bulk") = {1};

//n = 10;
//Mesh.MeshSizeMax = 1/n;
Transfinite Curve {1} = 10+1;
