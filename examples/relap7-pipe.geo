// RELAP-7 example 1: simple horizontal pipe, L = 2 m, 5 elements
Point(1) = {0, 0, 0};
Point(2) = {2, 0, 0};
Line(1) = {1, 2};
Transfinite Curve{1} = 5+1;

Physical Point("inlet") = {1};
Physical Point("outlet") = {2};
Physical Curve("pipe") = {1};
