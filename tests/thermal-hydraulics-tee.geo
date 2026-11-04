// two branches mixing into a main pipe in the x-y plane
Point(1) = {0, 0, 0};
Point(2) = {0, 1, 0};
Point(3) = {1, 0.5, 0};
Point(4) = {3, 0.5, 0};
Line(1) = {1, 3};
Line(2) = {2, 3};
Line(3) = {3, 4};
Transfinite Curve{1,2} = 5;
Transfinite Curve{3} = 9;

Physical Point("in1") = {1};
Physical Point("in2") = {2};
Physical Point("out") = {4};
Physical Curve("branch") = {1, 2};
Physical Curve("main") = {3};
