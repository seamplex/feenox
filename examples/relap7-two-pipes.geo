// RELAP-7 example 6: two horizontal pipes of 1 m each, 20 elements each,
// with different flow areas connected by a junction at x = 1 m
Point(1) = {0, 0, 0};
Point(2) = {1, 0, 0};
Point(3) = {2, 0, 0};
Line(1) = {1, 2};
Line(2) = {2, 3};
Transfinite Curve{1,2} = 20+1;

Physical Point("inlet") = {1};
Physical Point("junction") = {2};
Physical Point("outlet") = {3};
Physical Curve("pipe1") = {1};
Physical Curve("pipe2") = {2};
