// RELAP-7 example 5: vertical core channel, L = 3.865 m, 40 elements
Point(1) = {0, 0, 0};
Point(2) = {0, 0, 3.865};
Line(1) = {1, 2};
Transfinite Curve{1} = 40+1;

Physical Point("inlet") = {1};
Physical Point("outlet") = {2};
Physical Curve("channel") = {1};
