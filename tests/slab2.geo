Point(1) = {0, 0, 0};
Point(2) = {1, 0, 0};
Line(1) = {1, 2};

Physical Point("left") = {1};
Physical Point("right") = {2};
Physical Line("bulk") = {1};

Transfinite Curve {1} = 5+1; // 6 nodes = 5 elements
Mesh.ElementOrder = 2;
