Point(1) = {0.0, 0, 0};
Point(2) = {0.5, 0, 0};
Point(3) = {1.0, 0, 0};
Line(1) = {1, 2};
Line(2) = {2, 3};

Physical Point("left") = {1};
Physical Point("right") = {3};
Physical Line("metal") = {1};
Physical Line("plastic") = {2};

Transfinite Curve {1,2} = 5+1;
