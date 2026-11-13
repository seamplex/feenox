// RELAP-7 example 9: counter-flow heat exchanger made of two 1 m pipes
// with 10 elements each, separated by a flat wall
// the primary pipe goes from (0,0.02) to (1,0.02)
// the secondary pipe goes from (1,0) to (0,0), i.e. in the opposite direction
Point(1) = {0, 0.02, 0};
Point(2) = {1, 0.02, 0};
Point(3) = {1, 0, 0};
Point(4) = {0, 0, 0};
Line(1) = {1, 2};
Line(2) = {3, 4};
Transfinite Curve{1,2} = 10+1;

Physical Curve("primary") = {1};
Physical Curve("secondary") = {2};
Physical Point("primary_inlet") = {1};
Physical Point("primary_outlet") = {2};
Physical Point("secondary_inlet") = {3};
Physical Point("secondary_outlet") = {4};
